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

private:

    void InitializeConnection();
    void InitializeUI();

    void StartAutoState(bool bInAutoPlay);
    void EndAutoState();
    void StartAutoPlay();
    void EndAutoPlay();
    void AutoPause();
    void StepForward();
    void StepBack();

    bool bAutoState = false;
    bool bAutoPlay = false;

    static const QColor DefaultMapUnitColor;
    static const QColor HoverEnterDefaultMapUnitColor;

    static const int32_t AutoPlayIntervalTime;
    static const int32_t MapSize;

    QTimer* AutoPlayerTimer = nullptr;
    class QGridLayout* ContentLayout = nullptr;
    Maze Core;

    Ui::MazeWindow *ui;


};

#endif // MAZEWINDOW_H
