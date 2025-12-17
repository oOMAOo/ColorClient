#include "mainwindow.h"
#pragma execution_character_set("utf-8")
#include <QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    MainWindow w;
    w.show();
    return a.exec();
}
