#ifndef MODULESSEARCHDIALOG_H
#define MODULESSEARCHDIALOG_H


#include <QDialog>
#include <QMovie>
#include <QList>
#include <QTimer>
#include "analyzer.h"


namespace Ui {
class ModulesSearchDialog;
}



class ModulesSearchDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::ModulesSearchDialog *ui;

    Analyzer* mAnalyzer;

    QMovie mMovie;
    QTimer mTimer;
    int mSeconds;

public:
    explicit ModulesSearchDialog(Analyzer* analyzer, QWidget *parent = nullptr);
    ~ModulesSearchDialog();

public slots:
    void timerTick();
    void connectionFinished(ConnectionStatus status);
};


#endif // MODULESSEARCHDIALOG_H

