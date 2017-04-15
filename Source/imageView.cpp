
/**********************************************************************
**	SOURCE FILE:	imageView.cpp
**
**	PROGRAM:	Steganography
**
**	FUNCTIONS:
**    void dragEnterEvent(QDragEnterEvent *)
**    void dropEvent(QDropEvent *)
**    void dragMoveEvent(QDragMoveEvent *event)
**    ImageView(QWidget *parent = 0)
**    void setWindow(MainWindow *)
**
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
** Custom class of the QGraphicsView to allow drag and drops.
*************************************************************************/
#include <iostream>
#include <QDropEvent>
#include <QUrl>
#include <QErrorMessage>
#include <QMimeData>
#include <QDebug>
#include "imageView.h"

using namespace std;



/*****************************************************************
** Function: imageView
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
**          ImageView(QWidget *parent) : QGraphicsView(parent)
**            QWidget *parent -- Widget that the image view will reside in
**
** Returns: N/A (Constructor)
**
** Notes:
** Constructor for the image view.
**********************************************************************/
ImageView::ImageView(QWidget *parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
}



/*****************************************************************
** Function: dragEnterEvent
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
**          void dragEnterEvent(QDragEnterEvent *event)
**            QDragEnterEvent *event -- Event that is occuring
**
** Returns: void
**
** Notes:
** Allows objects to be dragged onto the image view and handled.
**********************************************************************/
void ImageView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {

        event->acceptProposedAction();

    }
}



/*****************************************************************
** Function: dragMoveEvent
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
**          void dragMoveEvent(QDragMoveEvent *event)
**             QDragMoveEvent *event -- Event that is occuring
**
** Returns: void
**
** Notes:
** Allows objects to be moved around via drag on the image view.
**********************************************************************/
void ImageView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

/*****************************************************************
** Function: dropEvent
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
**          void dropEvent(QDropEvent *event)
**            QDropEvent *event -- Event containing what is dropped
**
** Returns: void
**
** Notes:
** Handles items that are dropped onto the image view.
**********************************************************************/
void ImageView::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();    

    QString fileName = urls[0].fileName();

    //update the image
    window->updateImage(urls[0].toLocalFile(), fileName);



}


/*****************************************************************
** Function: setWindow
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
**          void setWindow(MainWindow *parent)
**             MainWindow *parent -- Sets the parent window as the main window
**
**
** Returns: void
**
** Notes:
** Handles items that are dropped onto the image view.
**********************************************************************/
void ImageView::setWindow(MainWindow *parent)
{
    this->window = parent;
}
