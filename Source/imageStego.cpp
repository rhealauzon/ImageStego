/**********************************************************************
**	SOURCE FILE:	imageStego.cpp
**
**	PROGRAM:	Steganography
**
**	FUNCTIONS:
** int embedSecret(QImage *, std::ifstream *, std::string, int, bool *, int)
** string retrieveSecret(QImage *embedImage, bool *)
** bool getNextPixelEmbed(QRgb *)
** bool getNextPixelRetrieve(QRgb *)
**
**
**	DATE:       September 23rd, 2016
**
**
**	DESIGNER:	Rhea Lauzon
**
**
**	PROGRAMMER: Rhea Lauzon
**
**	NOTES:
** Contains main stegonography functions of embedding and retrieving
** a secret in an image.
*************************************************************************/

#include <iostream>
#include <QRgb>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include "imageStego.h"
#include "imageManipulation.h"

using namespace std;

int width = 0;
int height = 0;
QImage *carrier;
QRgb *line;
bool firstPixelSet = false;


/*****************************************************************
** Function: embedBitsInPixel
**
** Date: September 24rd, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**          int embedSecret(QImage *c, ifstream *secret,
**          string secretName, int fileSize, bool *enabledBits, int numBitsPerChannel)
**             QImage *c -- Carrier image to embed the secret in
**             ifstream *secret -- File pointer to the secret
**             string secretName -- name of the secret's file
**             int fileSize -- size of the secret
**             bool *enabledBits -- pointer of positions that are available
**             int numBitsPerChannel -- Number of bits avail. per channel
**
**
** Returns:
**			int -- 0 on success, -1 on failure
**
** Notes:
** Loops through an image pixel by pixel and embeds the secret file
** via bits nto the specified bit positions in the RGB channels.
**********************************************************************/
int embedSecret(QImage *c, ifstream *secret, string secretName, int fileSize, bool *enabledBits, int numBitsPerChannel)
{
    //reset data
    width = 0;
    height = 0;
    firstPixelSet = false;


    //set our global carrier to the carrier from the GUI
    carrier = c;

    //initialize our pixel
    QRgb *pixel = new QRgb();

    //set our initial component to the file name since we want to write it first
    SecretComponent component = FILE_NAME;


    //set the line to the first line
    line = (QRgb *) carrier->scanLine(height);


    int numBitsRemainingInPixel = 0;
    int namePos = 0;

    //convert the file size to a string
    stringstream ss;
    ss << fileSize;
    string fileSizeStr = ss.str();

    char character;
    while (*secret)
    {
        switch (component)
        {
            case FILE_NAME:
            {
                //if we've placed all of the file name in the file set the next byte as nulls as a delmeter
                if (namePos == secretName.length())
                {
                    character = '\0';
                    component = FILE_SIZE;
                    namePos = 0;
                }
                else
                {
                    character = secretName[namePos];
                    namePos++;
                }
            }
            break;

            case FILE_SIZE:
            {
                //if we've placed all of the file name in the file set the next byte as nulls as a delmeter
                if (namePos == fileSizeStr.length())
                {
                    character = '\0';
                    component = RAW_DATA;
                    namePos = 0;
                }
                else
                {
                    character = fileSizeStr[namePos];
                    namePos++;
                }

            }
            break;

            case RAW_DATA:
            {
                secret->read(&character, 1);
            }
            break;
        }

        //convert the character to bits
        queue<int> bits;
        for (int i = 0; i < NUM_BITS; i++)
        {
            bits.push(GET_BIT_VALUE(character, NUM_BITS -i - 1));
        }

        //keep going until we've embedded all 8 bits of the byte
        while (!bits.empty())
        {
            if (numBitsRemainingInPixel == 0)
            {
                if (getNextPixelEmbed(pixel))
                {
                    numBitsRemainingInPixel = 3 * numBitsPerChannel;
                }
                else
                {
                    cout << "FILE WAS TOO BIG" << endl;
                    cout.flush();
                    return -1;
                }
            }
            //keep track of how many bits remain in this pixel
            numBitsRemainingInPixel -= embedBitsInPixel(pixel, &bits, enabledBits, numBitsPerChannel, numBitsRemainingInPixel);
        }
    }

    //do one last final embed incase of partially filled pixels
    getNextPixelEmbed(pixel);

    //save the file
    c->save("secret.bmp");

    //close the secret
    secret->close();

    return 0;
}



/*****************************************************************
** Function: retrieveSecret
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
**          string retrieveSecret(QImage *embedImage, bool *enabledBits)
**              QImage *embedImage -- image that has a secret embedded
**              bool *enabledBits -- bit positions that have embedded data
**
**
** Returns:
**			string -- name of the file or empty if no secret found
**
** Notes:
** Retrieves a secret image from an image based off the bit positions
** specified in each colour channel (RGB).
**********************************************************************/
string retrieveSecret(QImage *embedImage, bool *enabledBits)
{
    //reset data in case of last attempts
    width = 0;
    height = 0;
    firstPixelSet = false;


    //get the crarier image
    carrier = embedImage;

    //initialize a pixel
    QRgb *pixel = new QRgb();

    queue<int> *bitsRetrieved = new queue<int>();
    bitset<NUM_BITS> byteData;

    //start by finding the file name
    SecretComponent component = FILE_NAME;

    string fileName;
    string fileSizeStr;
    int fileSize = 999999999;
    bool delimeterFound = false;
    ofstream secret;
    int byteCounter = 0;

    while(byteCounter < fileSize)
    {
        //get the next pixel
        getNextPixelRetrieve(pixel);

        //fetch all the bits hidden in the RGB channel
        retrieveBitsFromPixel(bitsRetrieved, pixel, enabledBits);

        while (bitsRetrieved->size() >= 8)
        {
            for (int i = 0; i < NUM_BITS; i++)
            {
                byteData.set(NUM_BITS - i - 1, bitsRetrieved->front());
                bitsRetrieved->pop();
            }

            //turn the raw bits into a byte (character)
            char character = static_cast<unsigned char>(byteData.to_ulong());

            //look for our delimeter for data that isnt the raw data
            if (character == '\0' && component != RAW_DATA)
            {
                delimeterFound = true;
            }

            switch (component)
            {
                //first we need the file name to open a stream to it
                case FILE_NAME:
                {
                    if (delimeterFound)
                    {
                        secret = ofstream(fileName, ios_base::binary);
                        component = FILE_SIZE;
                        delimeterFound = false;
                        break;
                    }

                    //add a character to the file name
                    fileName += character;
                }
                break;

                //next we need the file size to know how much data to read
                case FILE_SIZE:
                {
                    if (delimeterFound)
                    {
                        //try converting what we found into a file size, if it fails we know there is no secret
                        try
                        {
                            fileSize = stoi(fileSizeStr);
                        }
                        catch (const std::invalid_argument& i)
                        {
                            remove(fileName.c_str());
                            return "";
                        }

                        //make sure we got a proper file size before continuing
                        if (fileSize <= 0)
                        {
                            remove(fileName.c_str());
                            return "";
                        }

                        component = RAW_DATA;
                        delimeterFound = false;
                        break;
                    }
                    //add a character to the file size string
                    fileSizeStr += character;
                }
                break;

                //read the raw data now that we know the name + size
                case RAW_DATA:
                {
                    //only write if the file size has not been met
                    if (byteCounter < fileSize)
                    {
                        //write the byte into the file
                        secret << character;
                        byteCounter++;
                    }
                }
                break;
            }

        }
    }

    //close the file
    secret.close();

    return fileName;

}


/*****************************************************************
** Function: getNextPixelEmbed
**
** Date: September 30th, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**          bool getNextPixelEmbed(QRgb * pix)
**              Qrgb *pix -- The current pixel pointer
**
**
** Returns:
**			bool -- True when a pixel can be retrieved
**                  False if there is no next pixel.
**
** Notes:
** Fetches the next pixel in the image and updates the data of the
** previous pixel with the manipulated data.
**********************************************************************/
bool getNextPixelEmbed(QRgb * pix)
{
    if (firstPixelSet)
    {
        //set the previous pixel with the updated pixel (manipulated with data)
        line[width] = *pix;

        //increment the pixel position
        width++;
        if (width == carrier->width())
        {
            width = 0;
            height++;
            line = (QRgb *) carrier->scanLine(height);
        }

        if (height > carrier->height())
        {
            return false;
        }

    }

    //fetch the RGB channels of the pixel
    *pix = carrier->pixel(width, height);

    firstPixelSet = true;
    return true;
}


/*****************************************************************
** Function: getNextPixelRetrieve
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
**          bool getNextPixelEmbed(QRgb * pix)
**              Qrgb *pix -- The current pixel pointer
**
**
** Returns:
**			bool -- True when a pixel can be retrieved
**                  False if there is no next pixel.
**
** Notes:
** Fetches the next pixel in the image.
**********************************************************************/
bool getNextPixelRetrieve(QRgb *pix)
{
    if (firstPixelSet)
    {
        //increment the pixel position
        width++;
        if (width == carrier->width())
        {
            width = 0;
            height++;
        }

        if (height > carrier->height())
        {
            return false;
        }
    }

    //fetch the RGB channels of the pixel
    *pix = carrier->pixel(width, height);

    firstPixelSet = true;
    return true;
}
