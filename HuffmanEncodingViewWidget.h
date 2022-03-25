#ifndef HUFFMANENCODINGVIEWWIDGET_H
#define HUFFMANENCODINGVIEWWIDGET_H

#include <QWidget>

namespace Ui {
class HuffmanEncodingViewWidget;
}

class HuffmanEncodingViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HuffmanEncodingViewWidget(QWidget *parent = nullptr);
    ~HuffmanEncodingViewWidget();

protected:

    void paintEvent(QPaintEvent *event);

private:
    Ui::HuffmanEncodingViewWidget *ui;
};

#endif // HUFFMANENCODINGVIEWWIDGET_H
