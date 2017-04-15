/**********************************************************************
**	SOURCE FILE:	main.cpp -  Runner
**
**	PROGRAM:	Steganography
**
**	FUNCTIONS:
**             int main(int argc, char *argv[])
**
**	DATE: 		September 22nd, 2016
**
**
**	DESIGNER:	Rhea Lauzon A00881688
**
**
**	PROGRAMMER: Rhea Lauzon A00881688
**
**	NOTES:
** Main runner of the GUI application.
*************************************************************************/
#include "mainWindow.h"
#include <QApplication>


/*****************************************************************
** Function: main
**
** Date: September 22nd, 2016
**
** Revisions:
**
**
** Designer: Rhea Lauzon
**
** Programmer: Rhea Lauzon
**
** Interface:
**			int main(int argc, char *argv[])
**              int argc -- Number of arguments
**              char *argv[] -- Array of command line arguments
**
** Returns:
**			int -- Status of success or failure
**
** Notes:
** Runner of the application. Sets up the GUI
*******************************************************************/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
