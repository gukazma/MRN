#include "MRNWidget.h"
#include "ui_MRNWidget.h"
#include <QFileDialog>
#include <QDesktopServices>
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
    connect(ui->pushButton_openFolder, SIGNAL(clicked(bool)), this, SLOT(slot_openFolder(bool)));
}

void MRNWidget::slot_scan(bool)
{
    if (ui->comboBox_dataVendor->currentText() == "Samrt3D") {}
}

void MRNWidget::slot_openFolder(bool) {
    if (ui->lineEdit_dataFolder->text().isEmpty()) return;
    QUrl _url = QUrl::fromLocalFile(ui->lineEdit_dataFolder->text());
    QDesktopServices::openUrl(_url);
}

MRNWidget::~MRNWidget()
{
    delete ui;
}

