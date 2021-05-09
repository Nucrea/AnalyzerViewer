#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H


#include <QDialog>
#include "patient.h"


namespace Ui {
class PatientDialog;
}

class PatientDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::PatientDialog *ui;

    Patient* mPatient;
    QImage* mPhoto;

public:
    explicit PatientDialog(QWidget *parent = nullptr);
    ~PatientDialog();

    void setPatient(Patient* patient);

    void acceptChanges();
    void setPhotoClicked();
};

#endif // PATIENTDIALOG_H
