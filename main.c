// rootming@live.cn
#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <malloc.h>
#include <PROCESS.H>

#define DIM 1920
#define _TH_NUM 10
#define PE	DIM / _TH_NUM
#define DM1 (DIM-1)
#define _sq(x) ((x)*(x)) // square
#define _cb(x) abs((x)*(x)*(x)) // absolute value of cube
#define _cr(x) (uint8_t)(pow((x),1.0/3.0)) // cube root

typedef struct _arg{
	int32_t id;
	int32_t begin;
	int32_t end;
	uint8_t *src;
}arg;

arg units[_TH_NUM];
volatile int32_t thread_id[_TH_NUM];
volatile int count;
uint8_t *raw;
FILE *fp;


void pixel_write(arg *unit){
	double a = 0, b = 0, c = 0, d = 0, n = 0;
	uint8_t *p;
	p = unit->src;
	printf("Thread %d start\n", unit->id);
	printf("PE %d\n", PE);
//#pragma loop(hint_parallel(8))
	for (int32_t j = unit->begin; j < unit->end; j++){
		for (int32_t i = 0; i < DIM; i++){
			a = 0, b = 0, c = 0, d = 0, n = 0;
			while ((c = a * a) + (d = b * b) < 4 && n++ < 880){
				b = 2 * a * b + j * 8e-9 - .645411;
				a = c - d + i * 8e-9 + .356888;
			}
			p[0] = (uint8_t)(255 * pow((n - 80) / 800, 3.)) & 255;
			p[1] = (uint8_t)(255 * pow((n - 80) / 800, .7)) & 255;
			p[2] = (uint8_t)(255 * pow((n - 80) / 800, .5)) & 255;
			p += 3;
		}
	}
	printf("Thread %d end\n", unit->id);
	thread_id[unit->id] = 1;
}

int main(void)
{

	if ((raw = malloc(sizeof(uint8_t)* DIM * DIM * 3)) == NULL){
		perror("malloc error");
		exit(1);
	}
	//raw = malloc(sizeof(uint8_t)* DIM * DIM * 3);
	fp = fopen("MathPic.ppm", "wb");
	fprintf(fp, "P6\n%d %d\n255\n", DIM, DIM);
	for (int32_t i = 0; i < _TH_NUM; i++){
		units[i].begin = i * PE;
		units[i].end = units[i].begin + PE;
		units[i].id = i;
		units[i].src = &raw[i * PE * DIM * 3];
		_beginthread(pixel_write, 0, &units[i]);
	}

	while (count != _TH_NUM){
		count = 0;
		for (int i = 0; i < _TH_NUM; i++){
			count += thread_id[i];
		}
		Sleep(100L);
	}
	fwrite(raw, 1, 3 * DIM * DIM, fp);
	fclose(fp);
	printf("done\n");
	getch();
	return 0;
}
