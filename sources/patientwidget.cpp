#include "patientwidget.h"
#include "ui_patientwidget.h"


PatientWidget::PatientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatientWidget)
{
    ui->setupUi(this);

    for (int i=0; i<5; ++i)
    {
        QListWidgetItem* item = new QListWidgetItem(ui->list_values);
        QWidget* widget = new QWidget();

        QHBoxLayout* layout = new QHBoxLayout();
        widget->setLayout(layout);
        layout->setContentsMargins(2, 0, 2, 0);
        layout->addWidget(new QLabel("Тест"));

        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::VLine); // Horizontal line
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        line->setStyleSheet("QWidget{ background-color: white; }");
        layout->addWidget(line);
        layout->addWidget(new QLabel("Тест"));
        item->setSizeHint(QSize(0, 25));

        ui->list_values->addItem(item);
        ui->list_values->setItemWidget(item, widget);
    }

    ui->list_values->setMinimumSize(0, 25*5 + 2);
    ui->list_values->setMaximumSize(1000000, 25*5 + 2);
}


void PatientWidget::setPatient(const Patient& patient)
{
    //ui->label_name->setText( patient.getName() );
    //ui->edit_date->setDate( patient.getDate() );
   // ui->label_photo->setPixmap( QPixmap::fromImage(patient.getPhoto()) );
}


PatientWidget::~PatientWidget()
{
    delete ui;
}
