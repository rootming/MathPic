#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#pragma warning( disable : 4996 )
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

#define _DIM 19200 * 2
#define _BOX 1920 * 1

size_t  dim = _DIM;
size_t  host_mem_alloc_size = _DIM * _DIM * 3 * sizeof(uint8_t);
size_t  dev_mem_alloc_size = _BOX * _BOX * 3 * sizeof(uint8_t);
uint8_t *raw;
size_t kernel_size;
int color = 0x26b2c1a;
char *kernel_file;
cl_platform_id platform_id[5];
clock_t start, end;
void loadKernel(const char *path, char **source_str, size_t *source_size);
int getProgramBuildInfo(cl_program program, cl_device_id device);
int getDevices();

int main()
{
	FILE *image;


	cl_device_id device_id = NULL;
	//	cl_device_id *devices;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem memobj = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	char platformName[64];
	char openclVersion[64];
	char devicesName[64];
	size_t nameLen;


	loadKernel("kernel.cl", &kernel_file, &kernel_size);

	if ((raw = malloc(host_mem_alloc_size)) == NULL){
		perror("malloc error");
		getchar();
		exit(1);
	}
	//for (uint32_t i = 0; i < host_mem_alloc_size; i += 3)
	//	memcpy(raw + i, (void*)&color, sizeof(uint8_t)* 3);

	const char fileName[] = "kernel.cl";
	image = fopen("MathPic.ppm", "wb");
	fprintf(image, "P6\n%d %d\n255\n", dim, dim);



	/* Get platform/device information */
	ret = clGetPlatformIDs(5, platform_id, &ret_num_platforms);		//获取平台数量
	ret = clGetPlatformIDs(ret_num_platforms, platform_id, NULL);	//存入platform_id


	printf("Find:%d platform(s)\n", ret_num_platforms);
	for (uint32_t i = 0; i < ret_num_platforms; i++){
		clGetPlatformInfo(platform_id[i], CL_PLATFORM_NAME, 64, platformName, &nameLen);
		clGetPlatformInfo(platform_id[i], CL_PLATFORM_VERSION, 64, openclVersion, &nameLen);
		printf("%d: name: %s %s\n", i, platformName, openclVersion);
	}
	/* Create OpenCL Context */
	ret = clGetDeviceIDs(platform_id[1], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);//choose platform
	ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, 64, devicesName, NULL);
	printf("name: %s\n", devicesName);
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);


	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create memory buffer*/
	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, dev_mem_alloc_size, NULL, &ret);
	if (memobj == NULL){
		printf("Memory object1 failed to create!\n");
		getchar();
		return;
	}

	/* Create Kernel program from the read in source */
	program = clCreateProgramWithSource(context, 1, (const char **)&kernel_file, &kernel_size, &ret);

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	getProgramBuildInfo(program, device_id);
	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "vecAdd", &ret);

	/* Set OpenCL kernel argument */


	size_t global_work_size = _BOX * _BOX;
	//size_t local_work_size =  _DIM;

	/* Execute OpenCL kernel */
	printf("Picture: %dx%d\n", _DIM, _DIM);
	printf("Start render...");
	start = clock();
	for (int pos = 0; pos < _DIM * _DIM; pos += _BOX *_BOX ){
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
		ret = clSetKernelArg(kernel, 1, sizeof(size_t), (void *)&dim);
		ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&pos);
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
		if (ret == 0)
			puts("Kernel success exec.\n");
		else
			puts("Kernel fail exec.\n");
		ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, dev_mem_alloc_size, raw + pos * 3, 0, NULL, NULL);
		if (ret == 0)
			puts("Data success read.\n");
		else
			puts("Data fail read.\n");
	}
	end = clock();
	/* Transfer result from the memory buffer */
	//ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, host_mem_alloc_size, raw, 0, NULL, NULL);

	/* Display result */

	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	printf("Render end, using %d ms\n", end - start);
	printf("Start write...\n");
	fwrite(raw, 1, 3 * dim * dim, image);
	fclose(image);
	printf("done\n");
	free(raw);
	free(kernel_file);
	getchar();
	return 0;
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

	/* 选取所有的platforms*/
	platforms = (cl_platform_id*)
		malloc(sizeof(cl_platform_id)* num_platforms);
	err = clGetPlatformIDs(num_platforms, platforms, NULL);
	if (err < 0) {
		perror("Couldn't find any platforms");
		exit(1);
	}

	//循环查看所有platforms的devices信息，一般intel和AMD的都可以有两个devices：CPU和显卡
	//如果是nvidia的就一般只有一个显卡device了。
	for (int j = 0; j < (int)num_platforms; j++)
	{
		printf("\nplatform %d\n", j + 1);
		/* 步骤和platforms的一样 */
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

		/*循环显示platform的所有device（CPU和显卡）信息。*/
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