#include "measuredialog.h"
#include "ui_measuredialog.h"
#include "modelwidget.h"
#include "main.h"
#include "analyzer.h"
#include "math.h"

#include <QMessageBox>


QString secondsToString(int seconds);


MeasureDialog::MeasureDialog(Model* model, Analyzer* analyzer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasureDialog)
{
    ui->setupUi(this);

    ui->label_rotating->setMovie(&mMovie);
    mMovie.setFileName( loadResource("gifs", "loading1.gif") );
    mMovie.setSpeed(100);

    mAnalyzer = analyzer;
    mModel = model;

    ui->widget_model->setModel(&mModelMeasure);
    //ui->widget_model->setAngles(90, 0);
    ui->widget_model->setAngles(30, 30);
    ui->widget_model->setShowPanel(false);
    ui->widget_model->setEnabled(false);

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(timerTick()));
    connect(ui->button_start, SIGNAL(clicked()), this, SLOT(start_clicked()));
    connect(ui->button_stop, SIGNAL(clicked()), this, SLOT(stop_clicked()));
    connect(this, SIGNAL(finished(int)), this, SLOT(stop_clicked()));

    connect(mAnalyzer, SIGNAL(measureFinished(MeasuringStatus)), this, SLOT(measureFinished(MeasuringStatus)));
    connect(mAnalyzer, SIGNAL(measureNewLayer(QVector<Vertex>)), this, SLOT(updateModelLayer(QVector<Vertex>)));

    mTimer.setInterval(1000);

    ui->button_start->setEnabled(true);
    ui->button_stop->setEnabled(false);
}


//--------------------------------------------------------------
//-- Buttons ---------------------------------------------------
//--------------------------------------------------------------

void MeasureDialog::pause_clicked()
{

}


void MeasureDialog::stop_clicked()
{
    mAnalyzer->stopAll();
    mMovie.stop();
    mTimer.stop();

    ui->label_status->setText("ОСТАНОВКА...");
    ui->label_status->setStyleSheet("QLabel{ background-color: red; }");

    ui->button_start->setEnabled(false);
    ui->button_stop->setEnabled(false);
}


void MeasureDialog::start_clicked()
{
    ui->button_start->setEnabled(false);
    ui->button_stop->setEnabled(true);

    mSeconds = 0;

    AnalyzerMeasureSettings settings;
    settings.flag_delta = ui->checkbox_delta->isChecked();
    settings.flag_emg = ui->checkbox_emg->isChecked();
    settings.layers_count = ui->spinbox_layers->value();
    settings.layer_step = ui->spinbox_step->value();

    if (settings.flag_delta) {
        ui->widget_model->setShowMode(ModelShowMode::STIFFNESS);
    } else {
        ui->widget_model->setShowMode(ModelShowMode::SKIN);
    }

    mModelMeasure.clear();
    mMovie.start();
    mTimer.start();
    mAnalyzer->startMeasure(settings);
}


//--------------------------------------------------------------
//-- Model measure ---------------------------------------------
//--------------------------------------------------------------

void MeasureDialog::updateModelLayer(QVector<Vertex> layer)
{
    mVertexes.push_back(layer);
    mModelMeasure.setVertexes(mVertexes);
    ui->widget_model->setModel(&mModelMeasure);
    ui->label_layer->setText( QString::number(mVertexes.count()) );
}


void MeasureDialog::measureFinished(MeasuringStatus status)
{
    close();

    if (status == MeasuringStatus::STATUS_ERROR) {
        QMessageBox msg(this);
        //msg.setStyleSheet("QLabel{ min-width: 200px; min-height: 100px; }");
        msg.setIcon(QMessageBox::Critical);
        msg.setWindowTitle("Ошибка");
        msg.setText("Ошибка измерения");
        msg.exec();
    }

    else if (status == MeasuringStatus::STATUS_STOPPED) {
        QMessageBox msg(this);
        msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("Внимание");
        msg.setText("Измерение было остановлено. Сохранить модель?");
        msg.exec();

        if (msg.result() == QMessageBox::Ok) {
            mModel->clear();
            mModel->setVertexes(mVertexes);
            mModel->interpolatePoly(1, 2, 1);
        }
    }

    else {
        mModel->clear();
        mModel->setVertexes(mVertexes);
        mModel->interpolatePoly(1, 2, 1);
    }
}


//--------------------------------------------------------------
//-- Timer -----------------------------------------------------
//--------------------------------------------------------------

void MeasureDialog::timerTick() {
    ui->label_time->setText( secondsToString(++mSeconds) );
}


QString secondsToString(int seconds)
{
    int hours = seconds/3600;
    int minutes = seconds/60 - hours*60;
    seconds = seconds - hours*3600 - minutes*60;

    QString str = ((hours < 10)? "0" : "") + QString::number(hours) + ':';
    str += ((minutes < 10)? "0" : "") + QString::number(minutes) + ':';
    str += ((seconds < 10)? "0" : "") + QString::number(seconds);

    return str;
}


MeasureDialog::~MeasureDialog()
{
    delete ui;
}
