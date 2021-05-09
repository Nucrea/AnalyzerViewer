#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "model.h"
#include "analyzer.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    Model mModel;
    Analyzer mAnalyzer;
    QProcess* mAnalyzerProcess;

    void initToolbar();

    void clickConnect();
    void clickMeasure();
    void clickNewPatient();
    void clickFindPatients();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void connectionFinished(ConnectionStatus status);
    void disconnected(DisconnectStatus status);
};


#endif // MAINWINDOW_H
