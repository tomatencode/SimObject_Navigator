#include "mainwindow.h"

#include <QObject>
#include <QApplication>
#include <QListWidget>

#include <QWidget>
#include<QGroupBox>
#include<QHBoxLayout>
#include<QPushButton>
#include<QString>
#include <QTranslator>
#include <QtCore>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    
    for (int i = 1; i<argc; ++i)
    {
        w.readSimObject(i, argv[i]);
    }

    w.show();

    return a.exec();
}
