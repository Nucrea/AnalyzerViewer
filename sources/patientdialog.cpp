#include "patientdialog.h"
#include "ui_patientdialog.h"

#include <QFileDialog>

PatientDialog::PatientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatientDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &PatientDialog::acceptChanges);
    connect(ui->button_photo, &QPushButton::clicked, this, &PatientDialog::setPhotoClicked);

    mPatient = nullptr;
    mPhoto = nullptr;
}


void PatientDialog::setPatient(Patient* patient)
{
    if (patient == nullptr) {
        return;
    }

    //QStringList names = patient->getName().split(' ');

    //ui->edit_last_name->setText( names[0] );
    //ui->edit_first_name->setText( names[1] );
   // ui->edit_patrynomic->setText( names[2] );
   // ui->edit_date->setDate( patient->getDate() );
   // ui->label_photo->setPixmap( QPixmap::fromImage(patient->getPhoto()) );
}


void PatientDialog::setPhotoClicked()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.png *.jpg)"));

    if ( dialog.exec() ) {
        QFile file(dialog.selectedFiles()[0]);

        if ( file.open(QFile::ReadOnly) ) {
            file.close();
            mPhoto = new QImage(dialog.selectedFiles()[0], "png");
            ui->label_photo->setPixmap( QPixmap::fromImage(*mPhoto) );
        }
    }
}


void PatientDialog::acceptChanges()
{
    if (mPatient == nullptr) {
        return;
    }

    QString lastName = ui->edit_last_name->text();
    QString firstName = ui->edit_first_name->text();
    QString patrynomic = ui->edit_patrynomic->text();

    if (mPhoto == nullptr || lastName.length() == 0
            || firstName.length() == 0 || patrynomic.length() == 0) {
        return;
    }

    //mPatient->setName(lastName + " " + firstName + " " + patrynomic);
    //mPatient->setDate(ui->edit_date->date());
   // mPatient->setPhoto(*mPhoto);
}


PatientDialog::~PatientDialog()
{
    delete ui;

    if (mPhoto != nullptr)
        delete mPhoto;
}
