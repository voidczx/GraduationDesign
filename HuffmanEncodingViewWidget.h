#ifndef HUFFMANENCODINGVIEWWIDGET_H
#define HUFFMANENCODINGVIEWWIDGET_H

#include <QWidget>
#include "qmap.h"

namespace Ui {
class HuffmanEncodingViewWidget;
}

class HuffmanEncodingViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HuffmanEncodingViewWidget(QWidget *parent = nullptr);
    ~HuffmanEncodingViewWidget();

    void ClearAll();

    void AddNodes(const QString& RootNode, const QString& InLeaf01, const QString& InLeaf02);
    void AddEdgeWord(const QString& InLineWord, const QString& InLineValue);
    void AddEncodingResult(const QString& InWord, const QString& InEncodingResult);

protected:

    void paintEvent(QPaintEvent *event);

private:

    void DrawSphere(QPainter& InPainter, const QPoint& InCenter);

    static int32_t SphereRadius;
    static int32_t WidgetMargin;
    static int32_t HorizontalDistance;
    static int32_t VerticalDistance;

    Ui::HuffmanEncodingViewWidget *ui;
    QMap<QString, QPoint> SphereMap;
    QMap<QString, QLine> LineMap;
    QMap<QString, QString> EdgeWordMap;
    QMap<QString, QString> EncodingResultMap;
    QPoint Start;
};

#endif // HUFFMANENCODINGVIEWWIDGET_H
