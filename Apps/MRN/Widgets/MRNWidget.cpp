#include "MRNWidget.h"
#include "ui_MRNWidget.h"
#include <QFileDialog>
MRNWidget::MRNWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MRNWidget)
{
    ui->setupUi(this);

    connect(ui->toolButton, &QToolButton::clicked, [&]() { 
            auto result = QFileDialog::getExistingDirectory();
            ui->lineEdit_dataFolder->setText(result);
        });
}

MRNWidget::~MRNWidget()
{
    delete ui;
}
