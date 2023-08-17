#include "MRNWidget.h"
#include "ui_MRNWidget.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <MRN/MRN.h>
#include <QMessageBox>
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
    if (ui->lineEdit_dataFolder->text().isEmpty()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please select data folder!"));
        return;
    }
    boost::filesystem::path path = ui->lineEdit_dataFolder->text().toLocal8Bit().constData();
    if (m_mrn) m_mrn.reset();

    if (ui->comboBox_dataVendor->currentText() == "Smart3D") {
        m_mrn = std::make_shared<MRN::MRN>(path, MRN::SoarscapeOSGB);
    }
    else {
        m_mrn = std::make_shared<MRN::MRN>(path);
    }

    if (m_mrn->check()) {
        QMessageBox::information(nullptr, tr("Inform"), tr("Data Format is right!"));
    }
    else {
        QMessageBox::critical(nullptr, tr("Error"), tr("Data Format is wrong!"));
    }
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

