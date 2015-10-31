#ifndef DRAWBASE_H
#define DRAWBASE_H

#include <QMutex>
#include <QThread>
#include <iostream>
#include <cstring>
#include <cmath>
#include <stdint.h>
#include "drawer.h"


using namespace std;


class Drawbase:public QThread
{
    Q_OBJECT
public:
    Drawbase(const Drawinfo &info);
    void drawBlock(pos_t *postion);
    void drawThread();
    void paintEvent(QPaintEvent *event);

private:
    uint32_t drawPixel(int i, int j);
    void run() Q_DECL_OVERRIDE;
    uint32_t DIM;
    uint32_t BLK;
    uint8_t *image;
    uint32_t *count;
    pos_t **posTree;
    QMutex *locker;
    uint32_t blkCount;
    QString result;

signals:
    void resultReady(const QString &s);


};

#endif // DRAWBASE_H
