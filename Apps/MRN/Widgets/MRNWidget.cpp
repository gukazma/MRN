#include "MRNWidget.h"
#include "ui_MRNWidget.h"
#include <QFileDialog>
MRNWidget::MRNWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MRNWidget)
{
    ui->setupUi(this);
    ui->comboBox_dataVendor->addItem("Smart3D");
    connect(ui->toolButton, &QToolButton::clicked, [&]() {
        auto result = QFileDialog::getExistingDirectory();
        ui->lineEdit_dataFolder->setText(result);
    });
    connect(ui->pushButton_scan, SIGNAL(clicked(bool)), this, SLOT(slot_scan(bool)));
}

void MRNWidget::slot_scan(bool)
{
    if (ui->comboBox_dataVendor->currentText() == "Samrt3D") {}
}


MRNWidget::~MRNWidget()
{
    delete ui;
}
