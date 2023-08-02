#include "MRNDialog.h"
#include "ui_MRNDialog.h"

MRNDialog::MRNDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MRNDialog)
{
    ui->setupUi(this);
}

MRNDialog::~MRNDialog()
{
    delete ui;
}
