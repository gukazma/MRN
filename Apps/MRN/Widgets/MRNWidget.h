#ifndef MRNWIDGET_H
#define MRNWIDGET_H

#include <QWidget>

namespace Ui {
class MRNWidget;
}

class MRNWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MRNWidget(QWidget *parent = nullptr);
    ~MRNWidget();

private:
    Ui::MRNWidget *ui;
};

#endif // MRNWIDGET_H
