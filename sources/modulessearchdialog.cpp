#include "modulessearchdialog.h"
#include "ui_modulessearchdialog.h"
#include <QSerialPortInfo>
#include <QTimer>
#include <QLabel>
#include <QThread>
#include <QElapsedTimer>
#include <QMimeData>
#include <QMessageBox>
#include "analyzer.h"
#include "main.h"


ModulesSearchDialog::ModulesSearchDialog(Analyzer* analyzer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModulesSearchDialog)
{
    ui->setupUi(this);

    //Setup "loading" gif
    mMovie.setFileName( loadResource("gifs", "loading1.gif") );
    ui->label_loading->setMovie(&mMovie);
    mMovie.start();

    //Setup seconds counter
    mSeconds = 0;
    connect(&mTimer, &QTimer::timeout, this, &ModulesSearchDialog::timerTick);
    mTimer.setInterval(1000);
    mTimer.start();

    mAnalyzer = analyzer;
    connect(analyzer, SIGNAL(connectionFinished(ConnectionStatus)), this, SLOT(connectionFinished(ConnectionStatus)));

    connect(this, &QDialog::finished, analyzer, &Analyzer::stopAll);

    analyzer->startConnect();
}


void ModulesSearchDialog::timerTick() {
    ui->label_seconds->setText(QString::number(++mSeconds));
}


void ModulesSearchDialog::connectionFinished(ConnectionStatus status)
{
    /*QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() <= 2000) {
        QApplication::processEvents();
    }*/

    close();

    if (status != ConnectionStatus::OK) {
        QMessageBox msg(this);
        //msg.setStyleSheet("QLabel{ min-width: 200px; min-height: 100px; }");
        msg.setIcon(QMessageBox::Critical);
        msg.setWindowTitle("Ошибка");
        msg.setText("Ошибка подключения.\n\nПроверьте соединение.");
        msg.exec();
    }
}


ModulesSearchDialog::~ModulesSearchDialog()
{
    delete ui;
}


