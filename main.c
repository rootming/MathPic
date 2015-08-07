#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <PROCESS.H>
#include <Windows.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#pragma warning( disable : 4996 )
#define DEBUG
#define MAX_SOURCE_SIZE (0x100000)

#define _DIM 480
#define _BOX 240


void loadKernel(const char *path, char **source_str, size_t *source_size);
int getProgramBuildInfo(cl_program program, cl_device_id device);
void init();

uint32_t  dim = _DIM;
uint32_t  box = _BOX;
uint32_t  host_mem_alloc_size = _DIM * _DIM * 3 * sizeof(uint8_t);
uint32_t  dev_mem_alloc_size = _BOX * _BOX * 3 * sizeof(uint8_t);
uint32_t count_nv = 0;
uint32_t count_ig = 0;
const char fileName[] = "kernel.cl";
uint8_t *raw;
size_t kernel_size;
char *kernel_file;
cl_platform_id platform_id[5];
clock_t start, end;
int getDevices();
void nv_unit(void *pos);
void in_unit(void *pos);
cl_device_id device_id[2];
//	cl_device_id *devices;
cl_context context_nvida = NULL;
cl_context context_intel = NULL;
cl_command_queue command_queue_nv = NULL;
cl_command_queue command_queue_in = NULL;
cl_mem memobj_nvida = NULL;
cl_mem memobj_intel = NULL;
cl_program program_nvidia = NULL;
cl_program program_intel = NULL;
cl_kernel kernel_nv = NULL;
cl_kernel kernel_in = NULL;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;
cl_int ret;
char platformName[64];
char openclVersion[64];
char devicesName[64];
size_t nameLen;
size_t global_work_size = _BOX * _BOX;
volatile int id[2];
enum PLATFROM { NVIDIA, INTEL };
uint32_t pos = 0;
int arg = 0;


int main()
{
	FILE *image;
	loadKernel("kernel.cl", &kernel_file, &kernel_size);

	if ((raw = malloc(host_mem_alloc_size)) == NULL){
		perror("malloc error");
		getchar();
		exit(1);
	}
	
	image = fopen("MathPic.ppm", "wb");
	fprintf(image, "P6\n%d %d\n255\n", dim, dim);



	init();
	printf("Picture: %dx%d\n", dim, dim);
	printf("Start render...\n");
	start = clock();

	do{
		if (id[NVIDIA] == 0 && pos < dim * dim){
			arg = pos;
			WaitForSingleObject((HANDLE)_beginthread(nv_unit, 0, (void *)&arg), CL_INFINITY);
			pos += box * box;
			//continue;
		}
		if (id[INTEL] == 0 && pos < dim * dim){
			arg = pos;
			WaitForSingleObject((HANDLE)_beginthread(in_unit, 0, (void *)&arg), CL_INFINITY);
			while (id[INTEL] == 1);
			pos += box * box;
			//continue;
		}

	

	} while (pos < dim * dim || id[INTEL] || id[NVIDIA]);

	end = clock();
	printf("Render end, using %d ms\n", end - start);
	
	//_sleep(10000);
	/* Finalization */
	ret = clFlush(command_queue_nv);
	ret = clFinish(command_queue_nv);
	ret = clFlush(command_queue_in);
	ret = clFinish(command_queue_in);
	ret = clReleaseKernel(kernel_nv);
	ret = clReleaseProgram(program_nvidia);
	ret = clReleaseKernel(kernel_in);
	ret = clReleaseProgram(program_intel);
	ret = clReleaseMemObject(memobj_nvida);
	ret = clReleaseMemObject(memobj_intel);
	ret = clReleaseCommandQueue(command_queue_nv);
	ret = clReleaseCommandQueue(command_queue_in);
	ret = clReleaseContext(context_nvida);
	ret = clReleaseContext(context_intel);
	printf("Start write...\n");
	fwrite(raw, 1, 3 * dim * dim, image);
	fclose(image);
	printf("done\n");
	free(raw);
	free(kernel_file);
	getchar();
	return 0;
}
void init()
{
	/* Get platform/device information */
	ret = clGetPlatformIDs(5, platform_id, &ret_num_platforms);		//��ȡƽ̨����
	ret = clGetPlatformIDs(ret_num_platforms, platform_id, NULL);	//����platform_id


	printf("Find:%d platform(s)\n", ret_num_platforms);
	for (uint32_t i = 0; i < ret_num_platforms; i++){
		clGetPlatformInfo(platform_id[i], CL_PLATFORM_NAME, 64, platformName, &nameLen);
		clGetPlatformInfo(platform_id[i], CL_PLATFORM_VERSION, 64, openclVersion, &nameLen);
		printf("%d: name: %s %s\n", i, platformName, openclVersion);
	}
	/* Create OpenCL Context */
	ret = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, 1, &device_id[0], &ret_num_devices);//choose platform
	ret = clGetDeviceInfo(device_id[0], CL_DEVICE_NAME, 64, devicesName, NULL);
	printf("Using devices: %s\n", devicesName);

	ret = clGetDeviceIDs(platform_id[1], CL_DEVICE_TYPE_GPU, 1, &device_id[1], &ret_num_devices);//choose platform
	ret = clGetDeviceInfo(device_id[1], CL_DEVICE_NAME, 64, devicesName, NULL);
	printf("Using devices: %s\n", devicesName);

	context_nvida = clCreateContext(NULL, 1, &device_id[0], NULL, NULL, &ret);
	context_intel = clCreateContext(NULL, 1, &device_id[1], NULL, NULL, &ret);


	/* Create Command Queue */
	command_queue_nv = clCreateCommandQueue(context_nvida, device_id[0], 0, &ret);
	command_queue_in = clCreateCommandQueue(context_intel, device_id[1], 0, &ret);

	/* Create memory buffer*/
	memobj_nvida = clCreateBuffer(context_nvida, CL_MEM_READ_WRITE, dev_mem_alloc_size, NULL, &ret);
	memobj_intel = clCreateBuffer(context_intel, CL_MEM_READ_WRITE, dev_mem_alloc_size, NULL, &ret);
	if (memobj_nvida == NULL || memobj_nvida == NULL){
		printf("Memory object failed to create!\n");
		getchar();
		return;
	}


	program_nvidia = clCreateProgramWithSource(context_nvida, 1, (const char **)&kernel_file, &kernel_size, &ret);
	ret = clBuildProgram(program_nvidia, 1, &device_id[0], NULL, NULL, NULL);
	getProgramBuildInfo(program_nvidia, device_id[0]);

	program_intel = clCreateProgramWithSource(context_intel, 1, (const char **)&kernel_file, &kernel_size, &ret);
	ret = clBuildProgram(program_intel, 1, &device_id[1], NULL, NULL, NULL);
	getProgramBuildInfo(program_intel, device_id[1]);


	kernel_nv = clCreateKernel(program_nvidia, "vecAdd", &ret);
	kernel_in = clCreateKernel(program_intel, "vecAdd", &ret);


	ret = clSetKernelArg(kernel_nv, 0, sizeof(cl_mem), (void *)&memobj_nvida);
	ret = clSetKernelArg(kernel_nv, 1, sizeof(size_t), (void *)&dim);
	ret = clSetKernelArg(kernel_in, 0, sizeof(cl_mem), (void *)&memobj_intel);
	ret = clSetKernelArg(kernel_in, 1, sizeof(size_t), (void *)&dim);
}
void nv_unit(void *pos)
{
	id[NVIDIA] = 1;
	uint32_t tmp = *(uint32_t *)pos;
	count_nv++;
	ret = clSetKernelArg(kernel_nv, 2, sizeof(uint32_t), (void *)&tmp);
	ret = clEnqueueNDRangeKernel(command_queue_nv, kernel_nv, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
#ifdef DEBUG
	if (ret == 0)
		printf("Kernel success exec.\n");
	else
		printf("Kernel fail exec.\n");
#endif
	ret = clEnqueueReadBuffer(command_queue_nv, memobj_nvida, CL_TRUE, 0, dev_mem_alloc_size, raw + tmp * 3, 0, NULL, NULL);
#ifdef DEBUG
	if (ret == 0)
		printf("Data success read.\n");
	else
		printf("Data fail read %d�� read pos:%d\n", ret, *(int *)pos);
	printf("NV Work:%d finish\n", count_nv);
#endif
	id[NVIDIA] = 0;
	
}

void in_unit(void *pos){
	id[INTEL] = 1;
	uint32_t tmp = *((uint32_t *)pos);
	count_ig++;
	ret = clSetKernelArg(kernel_in, 2, sizeof(uint32_t), (void *)&tmp);
	ret = clEnqueueNDRangeKernel(command_queue_in, kernel_in, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
#ifdef DEBUG
	if (ret == 0)
		printf("Kernel success exec.\n");
	else
		printf("Kernel fail exec:%d\n", ret);
#endif
	ret = clEnqueueReadBuffer(command_queue_in, memobj_intel, CL_TRUE, 0, dev_mem_alloc_size, raw + tmp * 3, 0, NULL, NULL);
#ifdef DEBUG
	if (ret == 0)
		printf("Data success read.\n");
	else
		printf("Data fail read %d�� read pos:%d\n", ret, *(int *)pos);
	printf("IG Work:%d finish\n", count_ig);
#endif
	id[INTEL] = 0;
}

void loadKernel(const char *path, char **source_str, size_t *source_size)
{
	FILE *fp;
	/* Load kernel source code */
	fp = fopen(path, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	*source_str = (char *)malloc(MAX_SOURCE_SIZE);
	*source_size = fread(*source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
}

int getProgramBuildInfo(cl_program program, cl_device_id device)
{
	size_t log_size;
	char *program_log;
	/* Find size of log and print to std output */
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
		0, NULL, &log_size);
	program_log = (char*)malloc(log_size + 1);
	program_log[log_size] = '\0';
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
		log_size + 1, program_log, NULL);
	printf("%s\n", program_log);
	free(program_log);
	return 0;
}

int getDevices() {
	/* Host/device data structures */
	cl_platform_id *platforms;
	cl_device_id *devices;
	cl_uint num_platforms;
	cl_uint num_devices, addr_data;
	cl_int i, err;

	/* Extension data */
	char name_data[48], ext_data[4096];

	err = clGetPlatformIDs(5, NULL, &num_platforms);
	if (err < 0) {
		perror("Couldn't find any platforms.");
		exit(1);
	}

	/* ѡȡ���е�platforms*/
	platforms = (cl_platform_id*)
		malloc(sizeof(cl_platform_id)* num_platforms);
	err = clGetPlatformIDs(num_platforms, platforms, NULL);
	if (err < 0) {
		perror("Couldn't find any platforms");
		exit(1);
	}

	//ѭ���鿴����platforms��devices��Ϣ��һ��intel��AMD�Ķ�����������devices��CPU���Կ�
	//�����nvidia�ľ�һ��ֻ��һ���Կ�device�ˡ�
	for (int j = 0; j < (int)num_platforms; j++)
	{
		printf("\nplatform %d\n", j + 1);
		/* �����platforms��һ�� */
		err = clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, 1, NULL, &num_devices);
		if (err < 0) {
			perror("Couldn't find any devices");
			exit(1);
		}

		/* Access connected devices */
		devices = (cl_device_id*)
			malloc(sizeof(cl_device_id)* num_devices);
		clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL,
			num_devices, devices, NULL);

		/*ѭ����ʾplatform������device��CPU���Կ�����Ϣ��*/
		for (i = 0; i<(int)num_devices; i++) {

			err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME,
				sizeof(name_data), name_data, NULL);
			if (err < 0) {
				perror("Couldn't read extension data");
				exit(1);
			}
			clGetDeviceInfo(devices[i], CL_DEVICE_ADDRESS_BITS,
				sizeof(ext_data), &addr_data, NULL);

			clGetDeviceInfo(devices[i], CL_DEVICE_EXTENSIONS,
				sizeof(ext_data), ext_data, NULL);

			printf("NAME: %s\nADDRESS_WIDTH: %u\nEXTENSIONS: %s\n\n",
				name_data, addr_data, ext_data);
		}
	}

	free(platforms);
	free(devices);
	return 0;
}