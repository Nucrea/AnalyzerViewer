#include "patientlistdialog.h"
#include "ui_patientlistdialog.h"

#include "patient.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>


static QWidget* createItemWidget();
static QFrame* createVLine();


PatientListDialog::PatientListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatientListDialog)
{
    ui->setupUi(this);


    for (int i=0; i<4; ++i)
    //for (auto name: Patient::listPatients())
    {
        QWidget* widget = createItemWidget();
        QListWidgetItem* item = new QListWidgetItem();

        item->setSizeHint( QSize(0, 160) );

        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, widget);
    }
}


void PatientListDialog::resizeEvent(QResizeEvent* e)
{
    QDialog::resizeEvent(e);
}


QWidget* PatientListDialog::newSeparatorWidget()
{
    QFrame* line = new QFrame();

    line->setMaximumWidth(1);
    line->setMinimumWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    return line;
}


PatientListDialog::~PatientListDialog()
{
    delete ui;
}


static QFrame* createVLine()
{
    QFrame* frame = new QFrame();
    frame->setFrameStyle(QFrame::VLine | QFrame::Plain);
    return frame;
}


static QWidget* createItemWidget()
{
    QLabel* labelName = new QLabel("Иванов Иван Иванович");
    QLabel* labelDate = new QLabel("21.04.1992");

    QLabel* labelPhoto = new QLabel();
    labelPhoto->setMinimumSize(140, 150);
    labelPhoto->setMaximumSize(140, 150);
    labelPhoto->setPixmap(QPixmap::fromImage(QImage(":/images/resources/images/noavatar.png")));
    labelPhoto->setScaledContents(true);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(labelName);
    layout->addWidget(labelDate);
    layout->addWidget(labelPhoto);

    QWidget* widget = new QWidget();
    widget->setLayout(layout);

    return widget;
}


