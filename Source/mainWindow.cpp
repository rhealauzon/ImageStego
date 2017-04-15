/**********************************************************************
**	SOURCE FILE:	mainWindow.cpp - GUI functionality
**
**	PROGRAM:	Steganography
**
**	FUNCTIONS:
**   MainWindow(QWidget *parent = 0)
**  ~MainWindow()
**   void determineSizeCapacity()
**   int checkFileSize(ifstream *)
**   void updateImage(QString, QString)
**   void clearCarrierImage()
**   void retrieveSecretImage()
**   void updateBitPositions()
**   void displayHelp()

**
**
**	DATE: 		September 23rd, 2016
**
**
**	DESIGNER:	Rhea Lauzon
**
**
**	PROGRAMMER: Rhea Lauzon
**
**	NOTES:
** This source file contains functionality to update and interact with the
** GUI dpslay.
*************************************************************************/
#include <iostream>
#include <QPointer>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QErrorMessage>
#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "imageStego.h"

using namespace std;


/*****************************************************************
** Function: MainWindow
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
**			void MainWindow(QWidget *parent)
**              QWidget *parent -- Pointer to the parent of the GUI
**
** Returns:
**			N/A (constructor)
**
** Notes:
** Sets up the GUI of the application. Adds all signals and slot
** functionality to the buttons.
*******************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //create the "scene" for displaying images
    scene = new QGraphicsScene(this);
    ui->mainImage->setScene(scene);

    ui->mainImage->setWindow(this);

    //setup the menu's slots
    connect(ui->actionClear_Image, SIGNAL(triggered(bool)), this, SLOT(clearCarrierImage()));
    connect(ui->actionRetrieve_Secret, SIGNAL(triggered(bool)), this, SLOT(retrieveSecretImage()));
    connect(ui->actionHow_to_Use, SIGNAL(triggered(bool)), this, SLOT(displayHelp()));

    //setup the slot on the "Clear carrier" button onClick
    connect(ui->clearCarrierBtn, SIGNAL(clicked()), this, SLOT(clearCarrierImage()));

    //setup the slot to retrieve the embedded secret
    connect(ui->retrieveBtn, SIGNAL(clicked()), this, SLOT(retrieveSecretImage()));

    //allow all the checkboxes to update the bytes available
    connect(ui->bit0,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));
    connect(ui->bit1,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));
    connect(ui->bit2,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));
    connect(ui->bit3,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));
    connect(ui->bit4,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));
    connect(ui->bit5,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));
    connect(ui->bit6,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));
    connect(ui->bit7,SIGNAL(toggled(bool)),this, SLOT(updateBitPositions()));



    //initialize the pointer to the carrier image
    carrierImage = new QImage();

    //initialize message boxes for displaying data
    stegoMsgBox.setText("Secret file has been embedded in the image.");
    stegoMsgBox.addButton("OK", QMessageBox::AcceptRole);
    openFile = stegoMsgBox.addButton(QMessageBox::Open);


    retrieveBox.setText("Secret file has been retrieved.");
    retrieveBox.addButton("OK", QMessageBox::AcceptRole);
    openSecretFile = retrieveBox.addButton(QMessageBox::Open);
}


/*****************************************************************
** Function: MainWindow()
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
**			~MainWindow()
**
** Returns:
**			N/A (Destructor)
**
** Notes:
** Deletes the main window from memory.
*******************************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}


/*****************************************************************
** Function: updateImage
**
** Date: September 24th, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**          void updateImage(QString filePath, QString localName)
**              QString filePath -- Location of the file
**              QString localName -- Name of the file without the path
**
** Returns:
**			void
**
** Notes:
** If this is the first image dropped into the window, the
** main image will be updated to it. If a second image is dropped,
** the steganographic algorithm will run and embed the second image
** into the first.
**********************************************************************/
void MainWindow::updateImage(QString filePath, QString localName)
{
    //get the number of bits and their positions
    updateBitPositions();

    //check if we have a carrier (first file dropped)
    if (carrierImage->isNull())
    {
        //load the new image into the view
        if (carrierImage->load(filePath))
        {
            currPixmap = QPixmap::fromImage(*carrierImage);
            scene->addPixmap(currPixmap);
            scene->setSceneRect(carrierImage->rect());

            //determine file size
            determineSizeCapacity();

            //update the GUI with the file path
            ui->displayImgPath->setText(filePath);

        }
        else
        {
            error.showMessage("Unable to open the file.");
        }
    }

    //second file dropped, so embed it since it is the secret
    else
    {
        secret = new ifstream(filePath.toStdString(), ios_base::binary);

        if (secret->is_open())
        {
            //check the file size first
            int secretSize = checkFileSize(secret);

            if (secretSize < maxSecretSize)
            {
                //embed the secret
                embedSecret(carrierImage, secret, localName.toStdString(), secretSize, enabledBits, numBitsPerChannel);


                //clear the scene to reload the changes
                scene->clear();

                //add the new image
                currPixmap = QPixmap::fromImage(*carrierImage);
                scene->addPixmap(currPixmap);
                scene->setSceneRect(carrierImage->rect());

                //update the GUI since the file is now manipulated
                ui->displayImgPath->setText("secret.bmp");

                stegoMsgBox.exec();


                if (stegoMsgBox.clickedButton() == openFile)
                {
                    //open the file
                    QDesktopServices::openUrl(QString::fromStdString("secret.bmp"));

                }

            }
            else
            {
                error.showMessage("Secret file is too large to embed.");
            }
        }
        else
        {
            error.showMessage("Unable to open the secret file.");
        }
    }

}

/*****************************************************************
** Function: determineSizeCapacity
**
** Date: September 24th, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**          void determineSizeCapacity()
**
** Returns:
**			void
**
** Notes:
** Determines the maximum amount of data that can be embedded
** into the carrier image based off the size and the number
** of bits per channnel.
**********************************************************************/
void MainWindow::determineSizeCapacity()
{
    if (!carrierImage->isNull())
    {
        maxSecretSize = (carrierImage->width() * carrierImage->height() * 3 * numBitsPerChannel) / 8;
        ui->fileSize->setText(QString::number(maxSecretSize));
    }
    else
    {
        ui->fileSize->setText(QString::number(0));
    }

}

/*****************************************************************
** Function: updateBitPositions
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
**          void updateBitPositions()
**
** Returns:
**			void
**
** Notes:
** Updates the "enabled bits" which are the bits that are
** available in each color channel of the image for manipulation,
** then recalculates the size capacity of the image.
**********************************************************************/
void MainWindow::updateBitPositions()
{
    numBitsPerChannel = 0;

    enabledBits[0] = ui->bit0->isChecked();
    enabledBits[1] = ui->bit1->isChecked();
    enabledBits[2] = ui->bit2->isChecked();
    enabledBits[3] = ui->bit3->isChecked();
    enabledBits[4] = ui->bit4->isChecked();
    enabledBits[5] = ui->bit5->isChecked();
    enabledBits[6] = ui->bit6->isChecked();
    enabledBits[7] = ui->bit7->isChecked();

    for (int i = 0; i < 8; i++)
    {
        if (enabledBits[i])
        {
            numBitsPerChannel++;
        }
    }

    determineSizeCapacity();

}

/*****************************************************************
** Function: checkFileSize
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
**          int checkFileSize(ifstream *file)
**              ifstream *file -- File pointer to check the size of
**
** Returns:
**			int -- the file size
**
** Notes:
** Determines the size of the file.
**********************************************************************/
int MainWindow::checkFileSize(ifstream *file)
{
    file->seekg(0, file->end);
    int fileSize = file->tellg();

    //rewind the file
    file->seekg(0, file->beg);


    return fileSize;
}

/*****************************************************************
** Function: clearCarrierImage
**
** Date: October 2nd, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**          void clearCarrierImage()
**
** Returns:
**			void
**
** Notes:
** Clears the carrier image off the GUI and resets all references to it.
**********************************************************************/
void MainWindow::clearCarrierImage()
{
    //set the carrier image back to null on the QImage and Pixmap
    carrierImage = new QImage();

    //clear the scene
    scene->clear();

    //update the file size counter
    determineSizeCapacity();

    //clear the file path of the image
    ui->displayImgPath->setText("");

}

/*****************************************************************
** Function: retrieveSecretImage
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
**          void retrieveSecretImage()
**
** Returns:
**			void
**
** Notes:
** A second file has been dropped into the view, so begin
** fetching the secret image out of the colour channels
** based off the bit positions set.
**********************************************************************/
void MainWindow::retrieveSecretImage()
{
    //only fetch a secret if there is a carrier image in the GUI
    if (!carrierImage->isNull())
    {
        //get the number of bits and their positions in case they have changed
        updateBitPositions();

        //retrieve the secret from the image if it exists
        string fileName = retrieveSecret(carrierImage, enabledBits);

        if (fileName.empty())
        {
            error.showMessage("No secret found.");
        }
        else
        {
            retrieveBox.exec();

            if (retrieveBox.clickedButton() == openSecretFile)
            {
                //open the file
                QDesktopServices::openUrl(QString::fromStdString(fileName));
            }
        }
    }
    else
    {
        error.showMessage("No image available to retrieve a secret from.");
    }
}


/*****************************************************************
** Function: displayHelp
**
** Date: October 4th, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**          void displayHelp()
**
** Returns:
**			void
**
** Notes:
** Displays a help message in a box.
**********************************************************************/
void MainWindow::displayHelp()
{
    error.showMessage("This is a tool used for embedding secret files into images (bitmaps) via stegonographic methods. First, drop an image into the large rectangle below. Next\
                set the bit positions you would like to use for the stegonographic alogrithms. If another file is dropped into the rectangle, it will be embedded into the \
                carrier and the output will be saved as secret.bmp. If you wish to retrieve a secret, simply press the Retrieve Secret button.");
}
