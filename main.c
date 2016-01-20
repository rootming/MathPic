// rootming@live.cn
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

//image size; block size; thread number
#define _DIM	9600
#define _BLK	128
#define _THUM	4

//#define DEBUG

const static uint32_t post_y = _BLK * _BLK / _DIM;
const static uint64_t image_pixel = _DIM * _DIM;
const static uint64_t block_pixel = _BLK * _BLK;
const static uint32_t size = _DIM % _BLK > 0 ? _DIM / _BLK + 1 : _DIM / _BLK;

typedef struct _pos_t
{
    uint32_t x, y;
} pos_t;

uint8_t *image;
pthread_mutex_t mutex;
const pos_t info = { .x = _DIM, .y = _DIM};
pos_t *pos_tree = NULL;
pos_t *pos_tree_bak = NULL;
pthread_t id[_THUM];
uint32_t count = 0;

uint32_t draw_pixel(int i, int j)
{
    uint32_t tmp;
    double a = 0, b = 0, c = 0, d = 0, n = 0;
    while ((c = a * a) + (d = b * b) < 4 && n++ < 880){
        b = 2 * a * b + j * 8e-9 - .645411;
        a = c - d + i * 8e-9 + .356888;
    } 
    tmp = (uint32_t)(255 * pow((n - 80) / 800, 3.));
    tmp |= (uint32_t)(255 * pow((n - 80) / 800, .7)) << 8;
    tmp |= (uint32_t)(255 * pow((n - 80) / 800, .5)) << 16;  
    return tmp;
}

void draw_block(pos_t *postion)
{
    uint32_t start_x, start_y;
    uint32_t end_x, end_y;
    uint32_t tp;					//temp pixel
    start_x = postion->x;
    start_y = postion->y;
    end_x = start_x + _BLK < _DIM ? start_x + _BLK : _DIM;
    end_y = start_y + _BLK < _DIM ? start_y + _BLK : _DIM;
    for(uint32_t y = start_y; y < end_y; y++){
        for(uint32_t x = start_x; x < end_x; x++){
            tp  = draw_pixel(x, y);
#ifdef DEBUG
            fprintf(stdout, "x = %u, y = %u\n", x, y);
#endif
            memcpy(image + 3 * (_DIM * y + x), &tp, 3);
        }
    }
}

void draw_thread()
{	
    pos_t *tmp;
    for(;;){
        pthread_mutex_lock(&mutex);
        if(count < size * size){
            count++;
            tmp = pos_tree++;
        }
        else{
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        draw_block(tmp);
    }
}

void cal_box()
{
    fprintf(stdout, "calculate postion...\n");
    if((pos_tree = malloc(sizeof(pos_t) * size * size)) == NULL){
        perror("malloc error");
        exit(1);
    }
    pos_tree_bak = pos_tree;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            pos_tree[i * size + j].x = i * _BLK;
            pos_tree[i * size + j].y = j * _BLK;
        }
    }

}

void show_box()
{
    printf("size :%d\n", size * size);
    for(int i = 0; i < size * size; i++){
        fprintf(stdout, "x = %d, y = %d\n", pos_tree[i].x, pos_tree[i].y);
    }
}

int main(void)
{

    FILE *fp;
    if ((image = malloc(sizeof(uint8_t) * image_pixel * 3)) == NULL){
        perror("malloc error");
        exit(1);
    }
    fp = fopen("MathPic.ppm", "wb");
    fprintf(fp, "P6\n%d %d\n255\n", _DIM, _DIM);
    fprintf(stdout, "image pixel is %lu\n", image_pixel);
    pthread_mutex_init(&mutex,NULL);
    cal_box();
    //show_box();
    fprintf(stdout, "Rendering...\n");
    time_t tStart = time(NULL);
    for (int i = 0; i < _THUM; i++){
        pthread_create(&id[i],NULL,(void *)draw_thread,NULL);  
    }
    for(int i = 0; i < _THUM; i++){
        pthread_join(id[i],NULL);
        fprintf(stdout, "Thread %d end\n", i);
    }
    time_t tRender = time(NULL);
    fprintf(stdout, "Time elapsed: %lds\n", tRender - tStart);
    fprintf(stdout, "Writing output...\n");
    fwrite(image, 1, image_pixel * 3, fp);
    fclose(fp);
    time_t tOutput = time(NULL);
    fprintf(stdout, "Time elapsed: %lds\n", tOutput - tRender);
    fprintf(stdout, "Total time elapsed: %lds\n", tOutput - tStart);
    free(image);
    free(pos_tree_bak);
    printf("done\n");
    return 0;
}
