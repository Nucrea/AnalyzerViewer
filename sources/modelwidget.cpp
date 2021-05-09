#include "modelwidget.h"
#include <QFile>
#include <GL/glu.h>


ModelWidget::ModelWidget(QWidget* w) : QOpenGLWidget(w)
{
    mModel = nullptr;
    mMouseLastPos = QPoint(0, 0);
    mShowMode = ModelShowMode::SKIN;

    mScale = 0.01;
    mRotX = 0;
    mRotY = 0;
    mRotZ = 0;

    mPanelShape.load(":/images/mode_shape.png");
    mPanelEmg.load(":/images/mode_emg.png");
    mPanelTougness.load(":/images/mode_toughness.png");

    mButtonShape.setCoords(60, 50, 101, 91);
    mButtonEmg.setCoords(168, 50, 209, 91);
    mButtonTougness.setCoords(276, 50, 317, 91);

    mDeltaColors.push_back( QColor(240, 41, 14) );
    mDeltaColors.push_back( QColor(240, 150, 14) );
    mDeltaColors.push_back( QColor(236, 240, 14) );
    mDeltaColors.push_back( QColor(160, 240, 14) );
    mDeltaColors.push_back( QColor(33, 240, 14) );
}


void ModelWidget::initializeGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    //Anti-aliasing
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
}


void ModelWidget::resizeGL(int width, int height)
{
    glMatrixMode(GL_PROJECTION); // устанавливает текущей проекционную матрицу
    glLoadIdentity();            // присваивает проекционной матрице единичную матрицу

    GLfloat ratio = (GLfloat)height/(GLfloat)width;

    if (width >= height) {
        glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -10.0, 1.0);
    } else {
        glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -10.0, 1.0);
    }

    glViewport(0, 0, (GLint)width, (GLint)height);

    mRatio = ratio;
    mWidth = width;
    mHeight = height;
}


void ModelWidget::drawBackground()
{
    glTranslatef(0, 0, 100*mScale); //Сдвиг СК

    float x_val = (mWidth >= mHeight)? 1.0/mRatio : 1.0;
    float y_val = (mWidth >= mHeight)? 1.0 : 1.0/mRatio;

    glBegin(GL_QUADS);
    glColor3ub(80, 80, 80);
    glVertex3f(-x_val, -y_val, 0);
    glVertex3f(x_val, -y_val, 0);
    glColor3ub(220, 220, 220);
    glVertex3f(x_val, y_val, 0);
    glVertex3f(-x_val, y_val, 0);
    glEnd();

    glTranslatef(0, 0, 100*mScale); //Сдвиг СК
}


void ModelWidget::drawModel()
{
    auto vertex = mModel->getVertexes();
    int layers = mModel->getLayers();
    int points = mModel->getPoints();

    if (layers==0 || points==0) {
        return;
    }

    glScalef(mScale, mScale, mScale);       // масштабирование
    glRotatef(mRotX, 1, 0, 0); // поворот вокруг оси Y
    glRotatef(mRotY, 0, 1, 0); // поворот вокруг оси Y
    glTranslatef(0, 0, -vertex[layers-1][0].z/2); //Сдвиг СК
    glLineWidth(2);

    //Линии и поверхности
    for (int c=0; c<2; ++c)
    {
        glPolygonMode(GL_FRONT_AND_BACK, (c == 0)? GL_LINE : GL_FILL);
        glBegin(GL_QUADS);

        for (int i=0; i<layers-1; ++i)
        {
            for (int j=0; j<points; ++j)
            {
                auto v1 = vertex[i][j];
                auto v2 = vertex[i+1][j];
                auto v3 = vertex[i+1][ (j+1)%points ];
                auto v4 = vertex[i][ (j+1)%points ];

                if (c == 0 ) {
                    glColor3ub(0, 0, 0);
                } else {
                    glColor3ub(0xFF, mModel->getColor()>>8, mModel->getColor()>>0);
                }

                if (c==1 && mShowMode!=ModelShowMode::SKIN)
                {
                    if (mShowMode==ModelShowMode::EMG) {
                        //glColor3ub(0, 0, v1.emg);
                        glColor3ub(30, 30, 240);
                        glVertex3d(v1.x, v1.y, v1.z);
                        //glColor3ub(0, 0, v2.emg);
                        glVertex3d(v2.x, v2.y, v2.z);
                        //glColor3ub(0, 0, v3.emg);
                        glVertex3d(v3.x, v3.y, v3.z);
                        //glColor3ub(0, 0, v4.emg);
                        glVertex3d(v4.x, v4.y, v4.z);
                    }
                    if (mShowMode==ModelShowMode::STIFFNESS)
                    {
                        float avg_delta = (v1.delta + v2.delta + v3.delta + v4.delta)/4;

                        QColor* color = nullptr;

                        if (avg_delta >= MAX_DELTA) {
                            color = &mDeltaColors.last();
                        } else {
                            float coeff = mDeltaColors.size()/MAX_DELTA;
                            color = &mDeltaColors[(int)(avg_delta*coeff)];
                        }

                        glColor3ub(color->red(), color->green(), color->blue());
                        glVertex3d(v1.x, v1.y, v1.z);
                        glVertex3d(v2.x, v2.y, v2.z);
                        glVertex3d(v3.x, v3.y, v3.z);
                        glVertex3d(v4.x, v4.y, v4.z);
                    }
                } else {
                    glVertex3d(v1.x, v1.y, v1.z);
                    glVertex3d(v2.x, v2.y, v2.z);
                    glVertex3d(v3.x, v3.y, v3.z);
                    glVertex3d(v4.x, v4.y, v4.z);
                }
            }
        }

        glEnd();
    }

    //Заднее отсечение
    glBegin(GL_POLYGON);
    glColor3ub(mModel->getColor()>>16, mModel->getColor()>>8, mModel->getColor()>>0);
    for (int i=0; i<points; ++i) {
        if (mShowMode == ModelShowMode::EMG) {
            //glColor3ub(0, 0, vertex[layers-1][i].emg);
        }
        if (mShowMode == ModelShowMode::STIFFNESS) {
            //glColor3ub(161, 29, 19);
        }
        glVertex3d(vertex[0][i].x, vertex[0][i].y, vertex[0][i].z);
    }
    glEnd();

    if (layers > 1)
    {
        //Переднее отсечение
        glBegin(GL_POLYGON);
        glColor3ub(mModel->getColor()>>16, mModel->getColor()>>8, mModel->getColor()>>0);
        for (int i=0; i<points; ++i) {
            if (mShowMode == ModelShowMode::EMG) {
                //glColor3ub(0, 0, vertex[layers-1][i].emg);
            }
            if (mShowMode == ModelShowMode::STIFFNESS) {
                //glColor3ub(161, 29, 19);
            }
            glVertex3d(vertex[layers-1][i].x, vertex[layers-1][i].y, vertex[layers-1][i].z);
        }
        glEnd();
    }
}


void ModelWidget::setModel(Model *model)
{
    mModel = model;
    update();
}


void ModelWidget::setAngles(int angleX, int angleY)
{
    mRotX = angleX;
    mRotY = angleY;
    update();
}


void ModelWidget::setShowMode(ModelShowMode mode) {
    mShowMode = mode;
    update();
}


void ModelWidget::setShowPanel(bool flag) {
    mFlagShowPanel = flag;
    update();
}


void ModelWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawBackground();
    if (mModel != nullptr && mModel->getLayers() > 0) {
        drawModel();
    }

    if (mFlagShowPanel)
    {
        glDisable(GL_DEPTH_TEST);
        QPainter painter(this);
        painter.beginNativePainting();

        switch(mShowMode) {
            case ModelShowMode::SKIN: painter.drawImage(QPointF(20, 20), mPanelShape); break;
            case ModelShowMode::EMG: painter.drawImage(QPointF(20, 20), mPanelEmg); break;
            case ModelShowMode::STIFFNESS: painter.drawImage(QPointF(20, 20), mPanelTougness); break;
        }

        QFont font("Arial", 10, QFont::Bold);
        QFontMetrics fm(font);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(QColor(0, 0, 0));
        painter.setFont(font);
        painter.drawText(30, 15, "Y:" + QString::number(mRotY, 'g', 4) );
        painter.drawText(110, 15, "X:" +QString::number(mRotX, 'g', 4) );

        double coeff = MAX_DELTA/mDeltaColors.size();
        painter.setPen(QColor(255, 255, 255));
        for (int i=0; i<mDeltaColors.size(); ++i)
        {
            char text[64];
            sprintf(text, "%.1f - %.1f mm", i*coeff, ((i+1)*coeff));
            painter.drawText(40, height() - 30*i - 15, text);
        }

        for (int i=0; i<mDeltaColors.size(); ++i)
        {
            QBrush brush(mDeltaColors[i]);
            QPen pen(Qt::black);
            QRectF color_rect(10, height() - 30*i - 35, 25, 25);
            painter.setBrush(brush);
            painter.setPen(pen);
            //painter.drawRect(color_rect);
            painter.drawRoundedRect(color_rect, 2, 2);
        }

        painter.endNativePainting();
        painter.end();
        glEnable(GL_DEPTH_TEST);
    }
}


void ModelWidget::wheelEvent(QWheelEvent* e)
{
    if (e->delta() > 0) {
        mScale += 0.001;
    } else {
        mScale -= 0.001;
    }

    if (mScale < 0.001) {
        mScale = 0.001;
    }

    update();
}


void ModelWidget::mousePressEvent(QMouseEvent* e)
{
    mMouseLastPos = e->pos();

    if ( mButtonShape.contains(e->pos()) ) {
        mShowMode = ModelShowMode::SKIN;
    }
    else if ( mButtonEmg.contains(e->pos()) ) {
        mShowMode = ModelShowMode::EMG;
    }
    if ( mButtonTougness.contains(e->pos()) ) {
        mShowMode = ModelShowMode::STIFFNESS;
    }

    update();
}


void ModelWidget::mouseMoveEvent(QMouseEvent* e)
{
    mRotX += 360*(e->pos().y() - mMouseLastPos.y()) / (float)mHeight;
    mRotY += 360*(e->pos().x() - mMouseLastPos.x()) / (float)mWidth;

    if (mRotX >= 360.0) { mRotX -= 360.0; }
    if (mRotY >= 360.0) { mRotY -= 360.0; }
    if (mRotX < 0) { mRotX += 360.0; }
    if (mRotY < 0) { mRotY += 360.0; }

    mMouseLastPos = e->pos();

    update();
}

