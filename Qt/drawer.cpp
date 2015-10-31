#include <stdint.h>
#include <QDebug>
#include <QObject>
#include "drawer.h"
#include "drawbase.h"

Drawer::Drawer(uint32_t imageSize, uint32_t blockSize, uint32_t threadNum)
{
    DIM = imageSize;
    BLK = blockSize;
    THUM = threadNum;
    count = 0;
    image = new uint8_t[DIM * DIM * 3];
    if(image == NULL){
        qDebug()<<"Alloc memory error";

    }
    postY = BLK * BLK / DIM;
    imagePixel = DIM * DIM;
    blockPixel = BLK * BLK;
    size = DIM % BLK > 0 ? DIM / BLK + 1 : DIM / BLK;
}

void Drawer::calcPost()
{
    qDebug()<<"calculate postion...\n";
    if((posTree = (pos_t *)new pos_t[size * size]) == NULL){
        qDebug()<<"alloc memory error";
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

void Drawer::drawThread()
{
    Drawinfo test;
    test.source = image;
    test.post = &posTree;
    test.l = &locker;
    test.c = &count;
    test.imageSize = DIM;
    test.blockSize = BLK;
    test.blockCount = size * size;

    qDebug()<<"Total draw block:"<<test.blockCount;
    qDebug()<<"Image length:"<<test.imageSize;
    qDebug()<<"Block length:"<<test.blockSize;
    Drawbase *workerThread = new Drawbase(test);
    //QObject::connect(workerThread, &Drawbase::resultReady, this, &Drawer::handleResults);
    //QObject::connect(workerThread, &Drawbase::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}
