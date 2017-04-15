#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <fstream>
#include <QErrorMessage>
#include <QMessageBox>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     void updateImage(QString, QString);

protected:


private:
    void determineSizeCapacity();    
    int checkFileSize(std::ifstream *);

    //display functionality
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QPixmap currPixmap;
    QGraphicsItem * currItem;
    int maxSecretSize;
    QErrorMessage error;
    QMessageBox stegoMsgBox;
    QMessageBox retrieveBox;
    QPushButton *openFile;
    QPushButton *openSecretFile;

    //stego properties
    QImage *carrierImage;
    std::ifstream * secret;

    int numBitsPerChannel;
    bool enabledBits[8];

private slots:
    void clearCarrierImage();
    void retrieveSecretImage();
    void updateBitPositions();
    void displayHelp();


};

#endif // MAINWINDOW_H
