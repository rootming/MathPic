//
//  drawer.m
//  test
//
//  Created by rootming on 16/4/26.
//  Copyright © 2016年 rootming. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "drawer.h"

@implementation Drawer

-(id)init
{
    self = [super init];
    _post_y = _BLK * _BLK / _DIM;
    _image_pixel = _DIM * _DIM;
    _block_pixel = _BLK * _BLK;
    _size = _DIM % _BLK > 0 ? _DIM / _BLK + 1 : _DIM / _BLK;
    _info.x = _DIM;
    _info.y = _DIM;
    _pos_tree = NULL;
    _pos_tree_bak = NULL;
    _pid = malloc(sizeof(pthread_t) * _THUM);
    _count = 0;
    return self;
}

-(uint32_t)drawPixel:(int)i secArg:(int)j
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

-(void)drawBlock:(pos_t *)postion
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
            tp = [self drawPixel:x secArg:y];
#ifdef DEBUG
            fprintf(stdout, "x = %u, y = %u\n", x, y);
#endif
            memcpy(_image + 3 * (_DIM * y + x), &tp, 3);
        }
    }
}

-(void)drawThread
{
    pos_t *tmp;
    for(;;){
        pthread_mutex_lock(&_mutex);
        if(_count < _size * _size){
            _count++;
            tmp = _pos_tree++;
        }
        else{
            pthread_mutex_unlock(&_mutex);
            break;
        }
        pthread_mutex_unlock(&_mutex);
        [self drawBlock:tmp];
    }
}

-(void)calcBox
{
    fprintf(stdout, "calculate postion...\n");
    if((_pos_tree = malloc(sizeof(pos_t) * _size * _size)) == NULL){
        perror("malloc error");
        exit(1);
    }
    _pos_tree_bak = _pos_tree;
    for(int i = 0; i < _size; i++){
        for(int j = 0; j < _size; j++){
            _pos_tree[i * _size + j].x = i * _BLK;
            _pos_tree[i * _size + j].y = j * _BLK;
        }
    }
    
}

-(void)showBox
{
    printf("size :%d\n", _size * _size);
    for(int i = 0; i < _size * _size; i++){
        fprintf(stdout, "x = %d, y = %d\n", _pos_tree[i].x, _pos_tree[i].y);
    }
}

-(void)allocImage
{
    if ((_image = malloc(sizeof(uint8_t) * _image_pixel * 3)) == NULL){
        perror("malloc error");
        exit(1);
    }
}

-(void)saveFile
{
    pos_t temp = { .x = 0, .y = 0 };
    FILE *fp = fopen("/Users/rootming/Desktop/MathPic.ppm", "wb");
    fprintf(fp, "P6\n%d %d\n255\n", _DIM, _DIM);
    fprintf(stdout, "image pixel is %llu\n", _image_pixel);
    //pthread_mutex_init(&_mutex, NULL);
    [self calcBox];
    [self allocImage];
    //show_box();
    fprintf(stdout, "Rendering...\n");
    time_t tStart = time(NULL);
//    for (int i = 0; i < _THUM; i++){
//        pthread_create(&_pid[i], NULL, (void)drawThread, NULL);
//    }
//    for(int i = 0; i < _THUM; i++){
//        pthread_join(_pid[i], NULL);
//        fprintf(stdout, "Thread %d end\n", i);
//    }
    //[self drawThread];
    [self drawBlock:&temp];
    time_t tRender = time(NULL);
    fprintf(stdout, "Time elapsed: %lds\n", tRender - tStart);
    fprintf(stdout, "Writing output...\n");
    fwrite(_image, 1, _image_pixel * 3, fp);
    fclose(fp);
    time_t tOutput = time(NULL);
    fprintf(stdout, "Time elapsed: %lds\n", tOutput - tRender);
    fprintf(stdout, "Total time elapsed: %lds\n", tOutput - tStart);
    free(_image);
    free(_pos_tree_bak);
    printf("done\n");
}

@end
