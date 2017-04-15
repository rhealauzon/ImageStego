#ifndef IMAGEMANIPULATION_H
#define IMAGEMANIPULATION_H

#include <QRgb>
#include <bitset>
#define NUM_BITS 8

int embedBitsInPixel(QRgb *, std::queue<int> *, bool *, int, int);
void retrieveBitsFromPixel(std::queue<int> *, QRgb *, bool *);

#endif // IMAGEMANIPULATION_H

