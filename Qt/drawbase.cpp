#include "drawbase.h"
#include <QDebug>

Drawbase::Drawbase(const Drawinfo &info)
{
        image = info.source;
        posTree = info.post;
        locker = info.l;
        count = info.c;
        DIM = info.imageSize;
        BLK = info.imageSize;
        blkCount = info.blockCount;
}

inline uint32_t Drawbase::drawPixel(int i, int j)
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

void Drawbase::drawBlock(pos_t *postion)
{
    uint32_t start_x, start_y;
    uint32_t end_x, end_y;
    uint32_t tp;					//temp pixel
    start_x = postion->x;
    start_y = postion->y;
    end_x = start_x + BLK < DIM ? start_x + BLK : DIM;
    end_y = start_y + BLK < DIM ? start_y + BLK : DIM;
    for(uint32_t y = start_y; y < end_y; y++){
        for(uint32_t x = start_x; x < end_x; x++){
            tp  = drawPixel(x, y);
#ifdef DEBUG
            clog<<"x ="<<c", "<<"y ="<<y<<endl;
#endif
            memcpy(image + 3 * (DIM * y + x), &tp, 3);
        }
    }
}


void Drawbase::run() Q_DECL_OVERRIDE {
    pos_t *tmp;
    for(;;){
        locker->lock();
        if(*count < blkCount){
            (*count)++;
            tmp = (*posTree)++;
        }
        else{
            locker->unlock();
            break;
        }
        locker->unlock();
        drawBlock(tmp);
        qDebug()<<"Work:"<<*count<<"done";
        qDebug()<<"next";
    }
    qDebug()<<"Finished";
    emit resultReady(result);
}
