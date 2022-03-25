#include "HuffmanEncodingViewWidget.h"
#include "ui_HuffmanEncodingViewWidget.h"

#include "qpainter.h"

HuffmanEncodingViewWidget::HuffmanEncodingViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HuffmanEncodingViewWidget)
{
    ui->setupUi(this);
}

HuffmanEncodingViewWidget::~HuffmanEncodingViewWidget()
{
    delete ui;
}

void HuffmanEncodingViewWidget::paintEvent(QPaintEvent *event){
    QPainter Painter(this);
    static int32_t X = 50;
    static int32_t Y = 50;
    Painter.drawEllipse(X, Y, 100, 100);
    X = X + 50;
    Y = Y + 50;
}

