#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>

#include "patientdialog.h"
#include "patientlistdialog.h"
#include "modulessearchdialog.h"
#include "measuredialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initToolbar();

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::clickNewPatient);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::clickFindPatients);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::clickConnect);
    connect(ui->actionAnalyse, &QAction::triggered, this, &MainWindow::clickMeasure);

    connect(&mAnalyzer, SIGNAL(connectionFinished(ConnectionStatus)),
            this, SLOT(connectionFinished(ConnectionStatus)));
    connect(&mAnalyzer, SIGNAL(disconnected(DisconnectStatus)), this, SLOT(disconnected(DisconnectStatus)));

    //Загрузка стандартной модели
    if ( mModel.loadFromCSV(":/models/model.csv") ) {
        mModel.interpolatePoly(1, 2, 2);
        ui->openGLWidget->setModel(&mModel);
        ui->openGLWidget->setAngles(30, 30);
    }
    ui->openGLWidget->setShowPanel(true);
}

//Обаботчик кнопки "новый пациент"
void MainWindow::clickNewPatient()
{
    PatientDialog dialog(this);
    dialog.exec();
}

//Обаботчик кнопки "найти пациента"
void MainWindow::clickFindPatients()
{
    PatientListDialog dialog(this);
    dialog.exec();
}

//Обаботчик кнопки "подключение"
void MainWindow::clickConnect()
{
    if ( mAnalyzer.isConnected() ) {
        mAnalyzer.startDisconnect();
        return;
    }

    //Запуск процесса-координатора
    //общение через UDP порты на 127.0.0.1
    mAnalyzerProcess = new QProcess(this);
    mAnalyzerProcess->start("analyzer.exe", QStringList());

    //Диалог с ожиданием подключения
    //В случае удачного подключения, запускается параллельный поток анализатора
    ModulesSearchDialog dialog(&mAnalyzer, this);
    dialog.exec();
}

//Обаботчик кнопки "начать измерение"
void MainWindow::clickMeasure()
{
    if ( !mAnalyzer.isConnected() ) {
        QMessageBox msg;
        msg.setWindowTitle("Ошибка");
        msg.setText("Анализатор не подключен");
        msg.setIcon(QMessageBox::Critical);
        msg.exec();
        return;
    }

    MeasureDialog dialog(&mModel, &mAnalyzer, this);
    dialog.exec();

    ui->openGLWidget->setModel(&mModel);
    ui->openGLWidget->setAngles(30, 30);
}

//Событие "измерение завершено"
void MainWindow::connectionFinished(ConnectionStatus status)
{
    QLabel* label = ui->toolBar->findChild<QLabel*>("label_status");
    label->setText(status==ConnectionStatus::OK? "Подключено" : "Не подключено");

    if (status != ConnectionStatus::OK) {
        mAnalyzerProcess->terminate();
        //delete(mAnalyzerProcess);
    }
}

//Событие "разрыв соединения"
void MainWindow::disconnected(DisconnectStatus status)
{
    QLabel* label = ui->toolBar->findChild<QLabel*>("label_status");
    label->setText("Не подключено");

    if (status != DisconnectStatus::ASKED)
    {
        QString text;
        switch(status) {
            case DisconnectStatus::PORT_ERROR: text = "Ошибка порта."; break;
            case DisconnectStatus::COMMAND_ERROR: text = "Ошибка команды."; break;
            default: text = "(?)."; break;
        }

        QMessageBox msg;
        msg.setWindowTitle("Ошибка");
        msg.setText("Соединение утеряно: " + text);
        msg.setIcon(QMessageBox::Critical);
        msg.exec();
    }

    mAnalyzerProcess->terminate();
    //delete(mAnalyzerProcess);
}

//Тулбар справа от всех кнопок
void MainWindow::initToolbar()
{
    QLabel* label = new QLabel("Не подключено", ui->toolBar);
    label->setObjectName("label_status");
    ui->toolBar->addWidget(label);

    /*QLabel* status = new QLabel("Состояние:");
    QLabel* status_text = new QLabel("Не подключено");
    QLabel* status_time = new QLabel("00:00:00");

    QLabel* status_img = new QLabel();
    status_img->setPixmap(QIcon(":/icons/resources/icons/cancel.svg").pixmap(32, 32));

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(status);
    vlayout->addWidget(status_text);
    vlayout->addWidget(status_time);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(status_img);
    hlayout->addLayout(vlayout);

    QFrame* frame = new QFrame();
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    frame->setLayout(hlayout);
    frame->setStyleSheet("QFrame{ background-color: white; }");

    ui->toolBar->addWidget(frame);*/
}


MainWindow::~MainWindow()
{
    delete ui;
}

