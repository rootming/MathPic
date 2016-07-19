//rootming@live.cn

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <cstdint>
#include <cstring>
#include <math.h>
#include "config.h"

using namespace std;

struct Pos_t
{
    uint32_t x, y;
    Pos_t() {}
    Pos_t(uint32_t x, uint32_t y) : x(x), y(y) { }
};

char *image;
queue <Pos_t> posList;
mutex mtx;

uint32_t drawPixel(uint32_t i, uint32_t j)
{
    uint32_t tmp;
    double a = 0, b = 0, c = 0, d = 0, n = 0;
    while ((c = a * a) + (d = b * b) < 4 && n++ < 880) {
        b = 2 * a * b + j * 8e-9 - .645411;
        a = c - d + i * 8e-9 + .356888;
    }
    tmp = (uint32_t)(255 * pow((n - 80) / 800, 3.));
    tmp |= (uint32_t)(255 * pow((n - 80) / 800, .7)) << 8;
    tmp |= (uint32_t)(255 * pow((n - 80) / 800, .5)) << 16;
    return tmp;
}

void drawUnit(Pos_t &pos)
{
    uint32_t startX, startY, endX, endY;
    startX = pos.x;
    startY = pos.y;
    uint32_t pixel;
    endX = _TARGET_SIDE > startX + _UNIT_SIDE ? startX + _UNIT_SIDE : _TARGET_SIDE;
    endY = _TARGET_SIDE > startY + _UNIT_SIDE ? startY + _UNIT_SIDE : _TARGET_SIDE;
    for (uint32_t y = startY; y < endY; y++) {
        for (uint32_t x = startX; x < endX; x++) {
            pixel = drawPixel(x, y);
            memcpy(image + 3 * (_TARGET_SIDE * y + x), &pixel, 3);
        }
    }

}

void calcUnitPosition()
{
    cout << "Calculating postion...\n";
    uint32_t count = _TARGET_SIDE % _UNIT_SIDE > 0 ? _TARGET_SIDE / _UNIT_SIDE + 1 : _TARGET_SIDE / _UNIT_SIDE;
    Pos_t temp;
    for (uint32_t i = 0; i < count; i++) {
        for (uint32_t j = 0; j < count; j++) {
            temp.x = i * _UNIT_SIDE;
            temp.y = j * _UNIT_SIDE;
            posList.push(temp);
        }
    }
    cout << "Calculate finish...\n";
}

void drawThread(int num)
{
    Pos_t temp;
    this_thread::sleep_for(std::chrono::milliseconds(num * 10));
    cout << "Thread:" << num << " is running..." << endl;
    for (;;) {
        mtx.lock();
        if (posList.empty()) {
            mtx.unlock();
            break;
        }
        temp = posList.front();
        posList.pop();
        mtx.unlock();
        drawUnit(temp);
    }
    this_thread::sleep_for(std::chrono::milliseconds(num * 10));
    cout << "Thread:" << num << " is finished." << endl;
}

int main()
{
    time_t start, write;
    ofstream file("MathPic.ppm", ios::binary | ios::trunc | ios::out);
    thread threads[_THREAD_LIMIT];

    image = new char[_TARGET_SIDE * _TARGET_SIDE * 3];
    if (image == NULL) {
        cerr << "Alloc memory errror.\n";
        exit(1);
    }
    file << "P6\n" << _TARGET_SIDE << " "<< _TARGET_SIDE << "\n255\n";
    cout <<  "Target image pixel is :" <<  _TARGET_SIDE * _TARGET_SIDE << endl;
    calcUnitPosition();
    start = time(NULL);
    for (int i = 0; i < _THREAD_LIMIT; i++) {
        threads[i] = thread(drawThread, i);
    }

    for (auto& it : threads) 
        it.join();

    write = time(NULL);
    cout << "Render is ended, using " << write - start << "s." << endl;
    file.write(image, _TARGET_SIDE * _TARGET_SIDE * 3);
    cout << "Write is ended, using " << time(NULL) - write << "s." << endl;
    cin.get();
    file.close();
}
