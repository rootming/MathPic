#ifndef DRAWBASE_H
#define DRAWBASE_H
#include <QWidget>
#include <iostream>
#include <cstring>
#include <cmath>
#include <stdint.h>

#define _DIM	9600
#define _BLK	128
#define _THUM	100

using namespace std;

typedef struct _pos_t
{
    uint32_t x, y;
} pos_t;

class Drawbase:public QWidget
{
public:
    Drawbase(QWidget *parent);
    Drawbase(uint32_t imageSize = _DIM, uint32_t blockSize = _BLK, uint32_t threadNum = _THUM);
    uint32_t drawPixel(int i, int j);
    void drawBlock(pos_t *postion);
    void calcPost();
    void virtual draw();

     void paintEvent(QPaintEvent *event);

private:
    uint8_t *image;
    uint32_t DIM;
    uint32_t BLK;
    uint32_t THUM;

    uint32_t postY;
    uint64_t imagePixel;
    uint64_t blockPixel;
    uint32_t size;

    pos_t *posTree;
    pos_t *posTreeBak;

};

#endif // DRAWBASE_H
