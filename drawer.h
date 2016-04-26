//
//  drawer.h
//  test
//
//  Created by rootming on 16/4/26.
//  Copyright © 2016年 rootming. All rights reserved.
//

#ifndef drawer_h
#define drawer_h

//image size; block size; thread number
#define _DIM	960
#define _BLK	960
#define _THUM	1


typedef struct _pos_t
{
    uint32_t x, y;
} pos_t;

@interface Drawer:NSObject

@property uint32_t post_y; //= _BLK * _BLK / _DIM;

@property uint64_t image_pixel; //= _DIM * _DIM;

@property uint64_t block_pixel; //= _BLK * _BLK;

@property uint32_t size; //= _DIM % _BLK > 0 ? _DIM / _BLK + 1 : _DIM / _BLK;

@property uint8_t *image;

@property pthread_mutex_t mutex;

@property pos_t info;// = { .x = _DIM, .y = _DIM};

@property pos_t *pos_tree; //= NULL;

@property pos_t *pos_tree_bak;// = NULL;

@property pthread_t *pid;

@property uint32_t count;// = 0;

-(id)init;

-(uint32_t)drawPixel:(int)i secArg:(int)j;

-(void)drawBlock:(pos_t *)postion;

-(void)drawThread;

-(void)calcBox;

-(void)showBox;

-(void)allocImage;

-(void)saveFile;




@end

#endif /* drawer_h */
