#ifndef ANALYZERPICTURE_H
#define ANALYZERPICTURE_H


#include <QWidget>
#include <QPixmap>
#include <QImage>


enum class DotColor
{
    RED,
    GREEN,
    YELLOW
};


class AnalyzerPicture : public QWidget
{
    QImage mImage;

    DotColor mColors[8];

public:
    AnalyzerPicture(QWidget* widget);

    void setDot(uint number, DotColor color);
    void setAllDots(DotColor color);

    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // ANALYZERPICTURE_H
