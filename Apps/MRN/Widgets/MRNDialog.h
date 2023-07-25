#ifndef MRNDIALOG_H
#define MRNDIALOG_H

#include <QDialog>

namespace Ui {
class MRNDialog;
}

class MRNDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MRNDialog(QWidget *parent = nullptr);
    ~MRNDialog();

private:
    Ui::MRNDialog *ui;
};

#endif // MRNDIALOG_H
