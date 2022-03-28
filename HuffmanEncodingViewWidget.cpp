#include "HuffmanEncodingViewWidget.h"
#include "ui_HuffmanEncodingViewWidget.h"

#include "qpainter.h"
#include "qdebug.h"

int32_t HuffmanEncodingViewWidget::SphereRadius = 30;
int32_t HuffmanEncodingViewWidget::WidgetMargin = 10;
int32_t HuffmanEncodingViewWidget::HorizontalDistance = 100 + SphereRadius * 2;
int32_t HuffmanEncodingViewWidget::VerticalDistance = 50 + SphereRadius * 2;

HuffmanEncodingViewWidget::HuffmanEncodingViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HuffmanEncodingViewWidget)
{
    ui->setupUi(this);
    Start = QPoint(SphereRadius + WidgetMargin, SphereRadius + WidgetMargin);
}

HuffmanEncodingViewWidget::~HuffmanEncodingViewWidget()
{
    delete ui;
}

void HuffmanEncodingViewWidget::ClearAll(){

}

void HuffmanEncodingViewWidget::AddNodes(const QString& RootNode, const QString& InLeaf01, const QString& InLeaf02){
    const bool bHasBothNode = SphereMap.contains(InLeaf01) && SphereMap.contains(InLeaf02);
    const bool bHasLeftNode = SphereMap.contains(InLeaf01) || SphereMap.contains(InLeaf02);
    if (bHasBothNode){
        const QString LeftWord = SphereMap[InLeaf01].x() < SphereMap[InLeaf02].x()? InLeaf01 : InLeaf02;
        const QString RightWord = SphereMap[InLeaf01].x() > SphereMap[InLeaf02].x()? InLeaf01 : InLeaf02;
        const QPoint LeftPosition = SphereMap[LeftWord];
        const QPoint RightPosition = SphereMap[RightWord];
        const int32_t NewX = (LeftPosition.x() + RightPosition.x()) / 2;
        const int32_t NewY = LeftPosition.y() > RightPosition.y()?
                    LeftPosition.y() + VerticalDistance : RightPosition.y() + VerticalDistance;
        const QPoint NewPosition(NewX, NewY);
        SphereMap.insert(RootNode, NewPosition);
        const QLine LeftLine(LeftPosition.x(), LeftPosition.y() + SphereRadius,
                             NewPosition.x(), NewPosition.y() - SphereRadius);
        const QLine RightLine(RightPosition.x(), RightPosition.y() + SphereRadius,
                              NewPosition.x(), NewPosition.y() - SphereRadius);
        LineMap.insert(LeftWord, LeftLine);
        LineMap.insert(RightWord, RightLine);
    }
    else if (bHasLeftNode){
        const QString LeftWord = SphereMap.contains(InLeaf01) ? InLeaf01 : InLeaf02;
        const QString RightWord = SphereMap.contains(InLeaf01) ? InLeaf02 : InLeaf01;
        const QPoint LeftPosition = SphereMap[LeftWord];
        const int32_t RightX = LeftPosition.x() + HorizontalDistance;
        const int32_t RightY = LeftPosition.y();
        const QPoint RightPosition(RightX, RightY);
        const int32_t RootX = (LeftPosition.x() + RightPosition.x()) / 2;
        const int32_t RootY = LeftPosition.y() + VerticalDistance;
        const QPoint RootPosition(RootX, RootY);
        SphereMap.insert(RightWord, RightPosition);
        SphereMap.insert(RootNode, RootPosition);
        const QLine LeftLine(LeftPosition.x(), LeftPosition.y() + SphereRadius,
                             RootPosition.x(), RootPosition.y() - SphereRadius);
        const QLine RightLine(RightPosition.x(), RightPosition.y() + SphereRadius,
                              RootPosition.x(), RootPosition.y() - SphereRadius);
        LineMap.insert(LeftWord, LeftLine);
        LineMap.insert(RightWord, RightLine);
    }
    else{
        const QPoint LeftPosition = Start;
        const QPoint RightPosition = QPoint(LeftPosition.x() + HorizontalDistance, LeftPosition.y());
        const int32_t RootX = (LeftPosition.x() + RightPosition.x()) / 2;
        const int32_t RootY = LeftPosition.y() + VerticalDistance;
        const QPoint RootPosition = QPoint(RootX, RootY);
        Start = QPoint(Start.x() + RightPosition.x() + HorizontalDistance * 2, Start.y());
        SphereMap.insert(InLeaf01, LeftPosition);
        SphereMap.insert(InLeaf02, RightPosition);
        SphereMap.insert(RootNode, RootPosition);
        const QLine LeftLine(LeftPosition.x(), LeftPosition.y() + SphereRadius,
                             RootPosition.x(), RootPosition.y() - SphereRadius);
        const QLine RightLine(RightPosition.x(), RightPosition.y() + SphereRadius,
                              RootPosition.x(), RootPosition.y() - SphereRadius);
        LineMap.insert(InLeaf01, LeftLine);
        LineMap.insert(InLeaf02, RightLine);
    }
}

void HuffmanEncodingViewWidget::paintEvent(QPaintEvent *event){
    if (event == nullptr){
        return;
    }
    QPainter Painter(this);
    for (auto Iter = SphereMap.begin(); Iter != SphereMap.end(); Iter++){
        DrawSphere(Painter, Iter.value());
    }
    for (auto Iter = LineMap.begin(); Iter != LineMap.end(); Iter++){
        Painter.drawLine(Iter.value());
    }
}

void HuffmanEncodingViewWidget::DrawSphere(QPainter& InPainter, const QPoint& InCenter){
    InPainter.drawEllipse(InCenter, SphereRadius, SphereRadius);
    if (InCenter.x() + SphereRadius > this->width()){
        this->resize(InCenter.x() + SphereRadius + WidgetMargin, this->height());
    }
    if (InCenter.y() + SphereRadius > this->height()){
        this->resize(this->width(), InCenter.y() + SphereRadius + WidgetMargin);
    }
}

