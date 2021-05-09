#ifndef PATIENTLISTDIALOG_H
#define PATIENTLISTDIALOG_H

#include <QDialog>

namespace Ui {
class PatientListDialog;
}

class PatientListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PatientListDialog(QWidget *parent = nullptr);
    ~PatientListDialog();

private:
    Ui::PatientListDialog *ui;

    QWidget* newSeparatorWidget();

protected:
    void resizeEvent(QResizeEvent* e) override;
};

#endif // PATIENTLISTDIALOG_H
