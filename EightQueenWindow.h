#ifndef EIGHTQUENEWINDOW_H
#define EIGHTQUENEWINDOW_H

#include <QWidget>

#include "EightQueen.h"

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

    void TryAddChess(const uint8_t& Row, const uint8_t& Col);
    void StepForward();
    void OnAddChessFail(const uint8_t& Row, const uint8_t& Col);
    void OnSuccess();
    void OnFail();

    Ui::EightQueenWindow *ui;

    const static uint8_t MapSize;
    const static int32_t AddChessFailWarningTime;
    EightQueen Core;
};

#endif // EIGHTQUENEWINDOW_H
