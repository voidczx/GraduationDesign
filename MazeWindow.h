#ifndef MAZEWINDOW_H
#define MAZEWINDOW_H

#include <QWidget>
#include "Maze.h"

namespace Ui {
class MazeWindow;
}

class MazeWindow : public QWidget
{
    Q_OBJECT

public:

    enum class CursorMode{
        ENone = 0,
        EStartPoint = 1,
        EFinishPoint = 2,
        EBlockPoint = 3,
        EClearPoint = 4,
        ENum,
    };

    explicit MazeWindow(QWidget *parent = nullptr);
    ~MazeWindow();

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

signals:

    void OnBackButtonClick();

private slots:

    void CloseSelfWindow();
    void BackButtonClick();

    void AutoStepForward();
    void AutoPlayPauseButtonClicked();
    void AutoStopButtonClicked();
    void AutoStepForwardButtonClicked();
    void AutoStepBackButtonClicked();

    void StartPointButtonClicked();
    void FinishPointButtonClicked();
    void BlockPointButtonClicked();
    void ClearPointButtonClicked();

private:

    void InitializeConnection();
    void InitializeUI();

    void ClearAllDynamic();
    void ClearAll();
    void UpdateMap();

    void StartAutoState(bool bInAutoPlay);
    void EndAutoState();
    void StartAutoPlay();
    void EndAutoPlay();
    void AutoPause();
    void StepForward();
    void StepBack();

    void ChangeToStartPointMode();
    void ChangeToFinishPointMode();
    void ChangeToBlockPointMode();
    void ChangeToClearPointMode();
    void ProcessChangeMapUnit(const int32_t& InRow, const int32_t& InCol);

    bool bAutoState = false;
    bool bAutoPlay = false;
    CursorMode CurrentCursorMode = CursorMode::ENone;

    static const QColor DefaultMapUnitColor;
    static const QColor HoverEnterDefaultMapUnitColor;
    static const QColor BlockMapUnitColor;
    static const QColor HoverEnterBlockMapUnitColor;
    static const QColor StartPointMapUnitColor;
    static const QColor HoverEnterStartPointMapUnitColor;
    static const QColor FinishPointMapUnitColor;
    static const QColor HoverEnterFinishPointMapUnitColor;

    static const int32_t AutoPlayIntervalTime;
    static const int32_t MapSize;

    QTimer* AutoPlayerTimer = nullptr;
    class QGridLayout* ContentLayout = nullptr;
    Maze Core;

    Ui::MazeWindow *ui;


};

#endif // MAZEWINDOW_H
