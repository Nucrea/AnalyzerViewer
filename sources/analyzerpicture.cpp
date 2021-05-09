#include "analyzerpicture.h"

#include <QPainter>
#include <QPaintEvent>
#include "main.h"
#include <math.h>


AnalyzerPicture::AnalyzerPicture(QWidget* widget) : QWidget(widget)
{
    mImage.load( loadResource("images", "analyzer.png") );
    setAllDots(DotColor::RED);
}


void AnalyzerPicture::setDot(uint number, DotColor color) {
    mColors[number] = color;
    update();
}


void AnalyzerPicture::setAllDots(DotColor color) {
    for(int i=0; i<8; ++i) {
        mColors[i] = color;
    }
    update();
}


void AnalyzerPicture::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    int space = 10;
    int radius = 10;
    int delta = 2*radius + space;

    QRect picRect(delta, delta, event->rect().width() - 2*delta, event->rect().height() - 2*delta);

    painter.drawImage(picRect, mImage);
    painter.drawRect(event->rect());

    for(int i=0; i<8; ++i)
    {
        double angle = i*M_PI_4 - M_PI_2;
        double cx = picRect.center().x();
        double cy = picRect.center().y();
        double len = (picRect.right() - picRect.left())/2 + radius + 4;
        QPointF center(cx + len*cos(angle), cy + len*sin(angle));

        switch(mColors[i]) {
            case DotColor::RED: painter.setBrush(Qt::red); break;
            case DotColor::GREEN: painter.setBrush(Qt::green); break;
            case DotColor::YELLOW: painter.setBrush(Qt::yellow); break;
        }

        painter.drawEllipse(center, radius, radius);
    }
}
