#include "MRNWidget.h"
#include "ui_MRNWidget.h"

MRNWidget::MRNWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MRNWidget)
{
    ui->setupUi(this);
}

MRNWidget::~MRNWidget()
{
    delete ui;
}
