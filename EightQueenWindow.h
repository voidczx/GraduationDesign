#ifndef EIGHTQUENEWINDOW_H
#define EIGHTQUENEWINDOW_H

#include <QWidget>

#include "qlabel.h"
#include "qstack.h"
#include "qmap.h"
#include "QTimer"

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
    void RemoveChessButtonClicked();
    void ViewMapButtonPressed();
    void ViewMapButtonReleased();

    void CloseSelf();

private:

    void InitializeConnection();
    void InitializeMap();

    void TryAddChess(const uint8_t& Row, const uint8_t& Col);
    void TryRemoveChess();
    void ViewMap();
    void RecoverMap();
    void StepForward();
    void OnSuccess();
    void OnFail();

    void BeginAutoState();
    void AutoPlay();
    void AutoStepForward();
    void AutoPause();
    void EndAutoState();

    Ui::EightQueenWindow *ui;

    const static uint8_t MapSize;
    const static int32_t AddChessFailWarningTime;
    const static int32_t AutoPlaySpaceTime;

    EightQueen Core;

    QStack<QLabel*> ChessStack; // TODO: move to core
    QMap<QLabel*, bool> MemoryMap; // TODO: delete and change RecoverMap()

    bool bAutoState = false;
    bool bAutoPlay = false;
    QTimer* AutoPlayerTimer = nullptr;
};

#endif // EIGHTQUENEWINDOW_H
