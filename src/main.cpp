#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif
    qDebug() <<QString::fromUtf8("中文测试");
    MainWindow w;
    w.show();
    return a.exec();
}
