#ifndef QPIXELWIDGET_H
#define QPIXELWIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>

#include "model.h"


enum ModelShowMode
{
    SKIN,
    EMG,
    STIFFNESS
};


class ModelWidget : public QOpenGLWidget
{
private:
    Model* mModel;

    float mScale;
    float mRotX;
    float mRotY;
    float mRotZ;

    QPoint mMouseLastPos;

    QImage mPanelShape;
    QImage mPanelEmg;
    QImage mPanelTougness;

    QRect mButtonShape;
    QRect mButtonEmg;
    QRect mButtonTougness;

    QVector<QColor> mDeltaColors;
    const float MAX_DELTA = 6.0;

    int mWidth;
    int mHeight;
    float mRatio;

    ModelShowMode mShowMode;
    bool mFlagShowPanel = false;

    void drawBackground();
    void drawModel();

public:
    ModelWidget(QWidget* widget);

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void wheelEvent(QWheelEvent* pe) override;
    void mousePressEvent(QMouseEvent* pe) override;
    void mouseMoveEvent(QMouseEvent* pe) override;

    void setModel(Model* model);
    void setAngles(int angleX, int angleY);
    void setShowMode(ModelShowMode mode);
    void setShowPanel(bool flag);
};

#endif // QMODELWIDGET_H
