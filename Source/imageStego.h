#ifndef IMAGESTEGO_H
#define IMAGESTEGO_H

#include <QImage>
#include <fstream>
#include <vector>

enum SecretComponent
{
    FILE_NAME,
    FILE_SIZE,
    RAW_DATA
};

int embedSecret(QImage *, std::ifstream *, std::string, int, bool *, int);
std::string retrieveSecret(QImage *embedImage, bool *);
bool getNextPixelEmbed(QRgb *);
bool getNextPixelRetrieve(QRgb *);

#define GET_BIT_VALUE(where, bit_number) (((where) & (1 << (bit_number))) >> bit_number) //this will return 0 or 1

#endif // IMAGESTEGO_H
