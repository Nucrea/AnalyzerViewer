#ifndef PATIENTWIDGET_H
#define PATIENTWIDGET_H

#include <QWidget>
#include "patient.h"


namespace Ui {
class PatientWidget;
}

class PatientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PatientWidget(QWidget *parent = nullptr);
    ~PatientWidget();

    void setPatient(const Patient& patient);

private:
    Ui::PatientWidget *ui;
};

#endif // PATIENTWIDGET_H
