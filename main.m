//
//  main.m
//  test
//
//  Created by rootming on 16/4/26.
//  Copyright © 2016年 rootming. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "drawer.h"






int main(int argc, const char * argv[]) {
    @autoreleasepool {

        Drawer *faker = [Drawer alloc];
        [faker saveFile];
    }
    return 0;
}
