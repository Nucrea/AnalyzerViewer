#ifndef MEASUREDIALOG_H
#define MEASUREDIALOG_H

#include <QDialog>
#include <QMovie>
#include <QTimer>
#include "model.h"
#include "analyzer.h"


namespace Ui {
class MeasureDialog;
}

class MeasureDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::MeasureDialog *ui;

    QTimer mTimer;
    QMovie mMovie;

    Model* mModel;
    Model mModelMeasure;

    Analyzer* mAnalyzer;

    QVector<QVector<Vertex>> mVertexes;

    int mSeconds = 0;

public:
    explicit MeasureDialog(Model* model, Analyzer* analyzer, QWidget *parent = nullptr);
    ~MeasureDialog();

public slots:
    void timerTick();
    void pause_clicked();
    void stop_clicked();
    void start_clicked();

    void updateModelLayer(QVector<Vertex> layer);
    void measureFinished(MeasuringStatus);
};


#endif // MEASUREDIALOG_H
