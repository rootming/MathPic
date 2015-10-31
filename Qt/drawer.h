#ifndef DRAWER_H
#define DRAWER_H
#include <QMutex>
#include <QWidget>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

#define _DIM	960
#define _BLK	128
#define _THUM	100

typedef struct
{
    uint32_t x, y;
} pos_t;


class Drawer:QObject
{
    Q_OBJECT

public:
    Drawer(uint32_t imageSize = _DIM, uint32_t blockSize = _BLK, uint32_t threadNum = _THUM);
    void calcPost();
    void drawThread();

public slots:
    void handleResults(){}

private:
    uint8_t *image;
    uint32_t DIM;
    uint32_t BLK;
    uint32_t THUM;
    uint32_t count;

    uint32_t postY;
    uint64_t imagePixel;
    uint64_t blockPixel;
    uint32_t size;

    pos_t *posTree;
    pos_t *posTreeBak;
    QMutex locker;

};

typedef struct
{
    uint8_t *source;
    pos_t **post;
    QMutex *l;
    uint32_t *c;
    uint32_t imageSize;
    uint32_t blockSize;
    uint32_t blockCount;

}Drawinfo;

#endif // DRAWER_H
