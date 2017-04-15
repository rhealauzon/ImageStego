/**********************************************************************
**	SOURCE FILE:	imageManipulation.cpp
**
**	PROGRAM:	Steganography
**
**	FUNCTIONS:
**  int embedBitsInPixel(QRgb *, queue<int> *, bool *, int, int)
**  void retrieveBitsFromPixel(queue<int> *, QRgb *, bool *)
**
**	DATE:        September 23rd, 2016
**
**
**	DESIGNER:	Rhea Lauzon
**
**
**	PROGRAMMER: Rhea Lauzon
**
**	NOTES:
** All functionality for manipulating the individual pixels
** and bits in an image.
*************************************************************************/
#include <iostream>
#include <QImage>
#include <queue>
#include "imageManipulation.h"


using namespace std;


/*****************************************************************
** Function: embedBitsInPixel
**
** Date: September 23rd, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**          int embedBitsInPixel(QRgb *pixel, queue<int> *bits, bool *enabledBits, int numBitsPerChannel, int bitsRemaining)
**              QRgb *pixel -- The pixel who's RGB channels will be manipulated
**              queue<int> *bits -- The bits of the secret to be embedded
**              bool *enabledBits -- The bits in each channel that are available for manipulation
**              int numBitsPerChannel -- Number of bits that are available per channel
**              int bitsRemaining -- The number of bits remaining to be used in this pixel
**
**
** Returns:
**			int -- The number of bits that were able to be insert
**                 into this particular pixel.
**
** Notes:
** Manipulates a pixel based on the bits that are available in each of
** the three colour channels (RGB). Since the available bits could be
** half the character 'A' and half the character 'B', we have to keep
** track of an offset into the bits that have already been filled
** with data from the secret.
**********************************************************************/
int embedBitsInPixel(QRgb *pixel, queue<int> *bits, bool *enabledBits, int numBitsPerChannel, int bitsRemaining)
{
    //determine the offset into the bits based off how many bits are remaining in this pixel
    int offset = numBitsPerChannel * 3 - bitsRemaining;
    int numBitsConsumed = 0;
    bool done = false;

    //get all the bits from the various channels
    QColor tempColor(*pixel);
    bitset<NUM_BITS> redBits(tempColor.red());
    bitset<NUM_BITS> greenBits(tempColor.green());
    bitset<NUM_BITS> blueBits(tempColor.blue());


    for (int channel = 0; channel < 3; channel++)
    {
        for (int i = 0; i < NUM_BITS; i++)
        {
            //go to the correct offset before setting bits
            if (enabledBits[NUM_BITS - i -1])
            {
                //decrement the offset until we are at the correct enabled bit that we havent used
                if (offset > 0)
                {
                    offset--;
                    continue;
                }

                //set the bit now that we are at the correct offset
                //NOTE: for "ecnryption" we flip all the bits
                if (channel == 0)
                {
                    redBits.set(NUM_BITS - i - 1, !((bool)bits->front()));
                }
                else if (channel == 1)
                {
                    greenBits.set(NUM_BITS - i - 1, !((bool)bits->front()));
                }
                else if (channel == 2)
                {
                    blueBits.set(NUM_BITS - i - 1, !((bool)bits->front()));
                }

                //remove the bit from the front
                bits->pop();
                numBitsConsumed++;

                if (bits->size() == 0)
                {
                    done = true;
                    break;
                }
            }
        }

        if (done)
        {
            break;
        }
    }

    //update the pixel with the new data
    QRgb newPixel;
    newPixel = qRgb(redBits.to_ulong(), greenBits.to_ulong(), blueBits.to_ulong());
    *pixel = newPixel;

    //return how many bits we used in the channel
    return numBitsConsumed;

}




/*****************************************************************
** Function: retrieveBitsFromPixel
**
** Date: October 3rd, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**      void retrieveBitsFromPixel(queue<int> *bitsRetrieved, QRgb *pixel, bool *enabledBits)
**          queue<int> *bitsRetrieved -- Pointer to a queue that contains the bits retrieved from the RGB channel
**          QRgb *pixel -- The pixel we are fetching data from
**          bool *enabledBits -- Pointer that contains the positions in a channel that we are using
**
**
**
** Returns:
**			void
**
** Notes:
** Retrieves bits from the 3 colour channels (RGB) based off the
** bit positions specified.
**********************************************************************/
void retrieveBitsFromPixel(queue<int> *bitsRetrieved, QRgb *pixel, bool *enabledBits)
{
    //get all the bits from the RGB channel
    QColor tempColor(*pixel);
    bitset<NUM_BITS> redBits(tempColor.red());
    bitset<NUM_BITS> greenBits(tempColor.green());
    bitset<NUM_BITS> blueBits(tempColor.blue());

    //go through each channel and pull the bits out of the specific positions
    for (int channelNum = 0; channelNum < 3; channelNum++)
    {
        //NOTE:: We flip all the bits because the original bits were flipped
        //for secrecy
        for (int bitPos = 0; bitPos < NUM_BITS; bitPos++)
        {
            if (enabledBits[NUM_BITS - bitPos - 1])
            {
                if (channelNum == 0)
                {
                    //fetch the bit from the red channel
                    bitsRetrieved->push(!redBits[NUM_BITS - bitPos - 1]);
                }
                else if (channelNum == 1)
                {
                    //fetch the bit from the green channel
                    bitsRetrieved->push(!greenBits[NUM_BITS - bitPos - 1]);
                }
                else if (channelNum == 2)
                {
                    //fetch the bit from the blue channel
                    bitsRetrieved->push(!blueBits[NUM_BITS - bitPos - 1]);
                }

            }
        }
    }
}
