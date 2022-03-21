#ifndef HUFFMANENCODINGWINDOW_H
#define HUFFMANENCODINGWINDOW_H

#include <QWidget>

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
};

#endif // HUFFMANENCODINGWINDOW_H
