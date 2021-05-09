#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>


QString loadResource(const QString& category, const QString& name)
{
    QFile file;

    QString filePath = ":/" + category + "/" + name;

    file.setFileName(name);
    if ( file.open(QFile::ReadOnly) ) {
        return name;
    }

    file.setFileName(filePath);
    if ( file.open(QFile::ReadOnly) ) {
        return filePath;
    }

    return QString();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    /*QFile file("style_green.css");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();
    file.close();
    //w.setStyleSheet(style);*/

    w.showMaximized();
    //w.show();

    return a.exec();
}
