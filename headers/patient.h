#ifndef PATIENT_H
#define PATIENT_H

#include <QDate>
#include <QImage>
#include <QMap>
#include <QDir>
#include "model.h"


struct Patient
{
    QString firstname;    //Имя
    QString surname;      //Фамиилия
    QString middlename;   //Отчество
    QImage photo;
    QDate born_date;
    QDate registration_date;
    int height;
    int weight;

    QVector<Model> models;

    static Patient* load(QString& firstname, QString& surname, QString& middlename, QDate born_date);
    static Patient* load(QString& firstname, QString& surname, QString& middlename);
    static bool save(Patient& patient);

    static const QVector<Patient> listPatients();
    static bool is_exists(QString& firstname, QString& surname, QString& middlename);

private:
    Patient();

    unsigned long sha256;
};


#endif // PATIENT_H
