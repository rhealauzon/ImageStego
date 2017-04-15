#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include "mainWindow.h"


class ImageView : public QGraphicsView
{

public:
    ImageView(QWidget *parent = 0);
    void setWindow(MainWindow *);

protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    void dragMoveEvent(QDragMoveEvent *event);


private:
    MainWindow * window;
};

#endif // IMAGEVIEW_H
