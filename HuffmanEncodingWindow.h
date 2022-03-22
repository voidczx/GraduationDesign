#ifndef HUFFMANENCODINGWINDOW_H
#define HUFFMANENCODINGWINDOW_H

#include <QWidget>
#include "HuffmanEncoding.h"

namespace Ui {
class HuffmanEncodingWindow;
}

class HuffmanEncodingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HuffmanEncodingWindow(QWidget *parent = nullptr);
    ~HuffmanEncodingWindow();

signals:

    void OnBackButtonClicked();

private slots:

    void BackButtonClicked();

    void CloseSelf();

private:

    void InitializeConnection();

    Ui::HuffmanEncodingWindow *ui;
    HuffmanEncoding Core;

};

#endif // HUFFMANENCODINGWINDOW_H
