#ifndef MRNWIDGET_H
#define MRNWIDGET_H

#include <QWidget>
#include <memory>
namespace MRN
{
class MRN;
}

namespace Ui {
class MRNWidget;
}

class MRNWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MRNWidget(QWidget* parent = nullptr);
    ~MRNWidget();

private slots:
    void slot_scan(bool);
    void slot_openFolder(bool);

private:
    Ui::MRNWidget* ui;
    std::shared_ptr<MRN::MRN> m_mrn;
};

#endif   // MRNWIDGET_H
