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
    void AutoPlayPauseButtonClicked();
    void AutoStopButtonClicked();
    void AutoStepForwardClicked();
    void AutoStepBackClicked();

    void CloseSelf();

private:

    void InitializeConnection();
    void InitializeMap();

    void TryAddChess(const uint8_t& Row, const uint8_t& Col);
    void TryRemoveChess();
    void AfterAddChessSuccess(const uint8_t& Row, const uint8_t& Col);
    void AfterAddChessFail(const uint8_t& Row, const uint8_t& Col);
    void AfterReduceChess();
    void ViewMap();
    void RecoverMap();
    void StepForward();
    void StepBack();
    void BeforeStepForward();
    void OnSuccess();
    void OnFail();

    void BeginAutoState(bool bAutoPlay);
    void AutoPlay();
    void AutoStepForward();
    void AutoPause();
    void EndAutoState();

    Ui::EightQueenWindow *ui;

    const static uint8_t MapSize;
    const static int64_t AddChessFailWarningTime;
    const static int64_t AutoPlaySpaceTime;
    const static int64_t AutoViewMapTime;

    EightQueen Core;

    QStack<QLabel*> ChessStack; // TODO: move to core
    QMap<QLabel*, bool> MemoryMap; // TODO: delete and change RecoverMap()

    class QGridLayout* ContentLayout = nullptr;

    bool bAutoState = false;
    bool bAutoPlay = false;
    QTimer* AutoPlayerTimer = nullptr;
};

#endif // EIGHTQUENEWINDOW_H
