#include "drawbase.h"
#include <QPainter>
#include <QPixmap>
#include <QLabel>

Drawbase::Drawbase(QWidget *parent):QWidget(parent)
{
    DIM = _DIM;
    BLK = _BLK;
    THUM =_THUM;
    image = (uint8_t *)new uint8_t(DIM * DIM * 3);
    postY = BLK * BLK / DIM;
    imagePixel = DIM * DIM;
    blockPixel = BLK * BLK;
    size = DIM % BLK > 0 ? DIM / BLK + 1 : DIM / BLK;
}

Drawbase::Drawbase(uint32_t imageSize, uint32_t blockSize, uint32_t threadNum)
{
    DIM = imageSize;
    BLK = blockSize;
    THUM = threadNum;
    image = (uint8_t *)new uint8_t[DIM * DIM * 3];
    postY = BLK * BLK / DIM;
    imagePixel = DIM * DIM;
    blockPixel = BLK * BLK;
    size = DIM % BLK > 0 ? DIM / BLK + 1 : DIM / BLK;
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

void Drawbase::calcPost()
{
    clog<<"calculate postion...\n"<<endl;
    if((posTree = (pos_t *)new pos_t[size * size]) == NULL){
        cerr<<"alloc memory error"<<endl;
        exit(1);
    }
    posTreeBak = posTree;
    for(uint32_t i = 0; i < size; i++){
        for(uint32_t j = 0; j < size; j++){
            posTree[i * size + j].x = i * BLK;
            posTree[i * size + j].y = j * BLK;
        }
    }
}

void Drawbase::draw()
{

}


void Drawbase::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        painter.drawLine(80, 100, 650, 500);
        painter.setPen(Qt::red);
        painter.drawRect(10, 10, 100, 400);
        painter.setPen(QPen(Qt::green, 5));
        painter.setBrush(Qt::blue);
        painter.drawEllipse(50, 150, 400, 200);
}
