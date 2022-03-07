#ifndef EIGHTQUENEWINDOW_H
#define EIGHTQUENEWINDOW_H

#include <QWidget>

namespace Ui {
class EightQueenWindow;
}

class EightQueenWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EightQueenWindow(QWidget *parent = nullptr);
    ~EightQueenWindow();

    bool eventFilter(QObject *watched, QEvent *event) override;

signals:

    void OnBackButtonClicked();

private slots:

    void BackButtonClicked();

    void CloseSelf();

private:

    void InitializeConnection();
    void InitializeMap();

    Ui::EightQueenWindow *ui;

    const uint8_t MapSize = 8;
};

#endif // EIGHTQUENEWINDOW_H
