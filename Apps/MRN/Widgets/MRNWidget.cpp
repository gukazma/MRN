#include "MRNWidget.h"
#include "ui_MRNWidget.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <MRN/MRN.h>
#include <QMessageBox>
#include <thread>
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
    connect(ui->pushButton_begin, SIGNAL(clicked(bool)), this, SLOT(slot_begin(bool)));
    connect(this,
            SIGNAL(signal_setProgress(int, QString)),
            this, SLOT(slot_setProgress(int, QString)));
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
    if (ui->lineEdit_dataFolder->text().isEmpty()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please select data folder!"));
        return;
    }
    boost::filesystem::path folder = ui->lineEdit_dataFolder->text().toLocal8Bit().constData();
    QUrl                    _url   = QUrl::fromLocalFile(folder.parent_path().generic_path().string().c_str());
    QDesktopServices::openUrl(_url);
}

void MRNWidget::slot_begin(bool) {
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
    }
    else {
        QMessageBox::critical(nullptr, tr("Error"), tr("Data Format is wrong!"));
        return;
    }
    ui->pushButton_begin->setEnabled(false);
    std::thread thread([&]() {
        QString message;
        emit this->signal_setProgress(0, tr("Calculating tile array."));
        m_mrn->calculateTileArray();
        emit this->signal_setProgress(30, tr("Tile array calculated."));
        emit this->signal_setProgress(40, tr("Begin merge."));
        m_mrn->merge();
        emit this->signal_setProgress(70, tr("Tile merged."));
        emit this->signal_setProgress(80, tr("Begin saving tile array."));
        m_mrn->writeTile(); 
        emit this->signal_setProgress(100, tr("Tile array saved."));
        });
    thread.detach();
}

void MRNWidget::slot_setProgress(int value_, QString message_)
{
    ui->progressBar->setValue(value_);
    ui->plainTextEdit->appendPlainText(message_);
    ui->pushButton_begin->setEnabled(false);
    if (value_ == 100)
    {
        ui->pushButton_begin->setEnabled(true);
        QMessageBox::information(nullptr, tr("Inform"), tr("Merge success!"));
    }
}

MRNWidget::~MRNWidget()
{
    delete ui;
}

