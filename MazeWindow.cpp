#include "MazeWindow.h"
#include "ui_MazeWindow.h"

#include "QTimer"
#include "QGridLayout"
#include "QLabel"
#include "QDebug"
#include "QMouseEvent"
#include "QCursor"
#include "QMessageBox"

const QColor MazeWindow::DefaultMapUnitColor = QColor(0xbfbfbf);
const QColor MazeWindow::HoverEnterDefaultMapUnitColor = QColor(0xdfdfdf);
const QColor MazeWindow::BlockMapUnitColor = QColor(0x000001);
const QColor MazeWindow::HoverEnterBlockMapUnitColor = QColor(0x606061);
const QColor MazeWindow::StartPointMapUnitColor = QColor(0xFFCC33);
const QColor MazeWindow::HoverEnterStartPointMapUnitColor = QColor(0xFFDD66);
const QColor MazeWindow::FinishPointMapUnitColor = QColor(0xFF99CC);
const QColor MazeWindow::HoverEnterFinishPointMapUnitColor = QColor(0xFFCCFF);
const QColor MazeWindow::CurrentMapUnitColor = QColor(0xFFFFFF);
const QColor MazeWindow::OpenMapUnitColor = QColor(0x00FF00);
const QColor MazeWindow::CloseMapUnitColor = QColor(0xFF0000);
const QColor MazeWindow::PathMapUnitColor = QColor(0x0000FF);

const int32_t MazeWindow::AutoPlayIntervalTime = 800;
const int32_t MazeWindow::MapSize = 19;

MazeWindow::MazeWindow(QWidget *parent) :
    QWidget(parent),
    Core(MapSize, MapSize),
    ui(new Ui::MazeWindow)
{
    ui->setupUi(this);
    InitializeConnection();
    InitializeUI();
}

MazeWindow::~MazeWindow()
{
    qDebug("[MazeWindow::~MazeWindow()] Maze UI has been destroyed.");
    delete ui;
}

bool MazeWindow::eventFilter(QObject *watched, QEvent *event){
    if (QLabel* EventLabel = qobject_cast<QLabel*>(watched)){
        if (event->type() == QEvent::Type::Enter){
            int32_t CurrentRow = -1;
            int32_t CurrentCol = -1;
            Core.GetCurrentPointPosition(CurrentRow, CurrentCol);
            if (ContentLayout != nullptr){
                int32_t Index = ContentLayout->indexOf(EventLabel);
                int32_t LabelRow = Index / MapSize;
                int32_t LabelCol = Index % MapSize;
                if (LabelRow == CurrentRow && LabelCol == CurrentCol){
                    return true;
                }
                Maze::MazeUnitType LabelUnitType = Core.GetUnitType(LabelRow, LabelCol);
                ChangeLabelToHoverEnterColorByUnitType(EventLabel, LabelUnitType);
                return true;
            }
        }
        else if (event->type() == QEvent::Type::Leave){
            int32_t CurrentRow = -1;
            int32_t CurrentCol = -1;
            Core.GetCurrentPointPosition(CurrentRow, CurrentCol);
            if (ContentLayout != nullptr){
                int32_t Index = ContentLayout->indexOf(EventLabel);
                int32_t LabelRow = Index / MapSize;
                int32_t LabelCol = Index % MapSize;
                if (LabelRow == CurrentRow && LabelCol == CurrentCol){
                    return true;
                }
                Maze::MazeUnitType LabelUnitType = Core.GetUnitType(LabelRow, LabelCol);
                ChangeLabelToDefaultColorByUnitType(EventLabel, LabelUnitType);
                return true;
            }
        }
        else if (event->type() == QEvent::Type::MouseButtonPress){
            QMouseEvent* MouseEvent = dynamic_cast<QMouseEvent*>(event);
            if (MouseEvent != nullptr){
                if (MouseEvent->button() == Qt::LeftButton){
                    int32_t Index = ContentLayout->indexOf(EventLabel);
                    int32_t LabelRow = Index / MapSize;
                    int32_t LabelCol = Index % MapSize;
                    ProcessChangeMapUnit(LabelRow, LabelCol);
                    return true;
                }
            }
        }
        return false;
    }
    return QWidget::eventFilter(watched, event);
}

void MazeWindow::CloseSelfWindow(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}

void MazeWindow::BackButtonClick(){
    emit(OnBackButtonClick());
}

void MazeWindow::AutoStepForward(){
    StepForward();
}

void MazeWindow::AutoPlayPauseButtonClicked(){

    if (bAutoState){
        if (bAutoPlay){
            AutoPause();
        }
        else{
            StartAutoPlay();
        }
    }
    else{
        StartAutoState(true);
    }

}

void MazeWindow::AutoStopButtonClicked(){

    if (!bAutoState){
        return;
    }
    EndAutoState();

}

void MazeWindow::AutoStepForwardButtonClicked(){

    if (!bAutoState){
        StartAutoState(false);
    }
    if (bAutoPlay){
        AutoPause();
    }
    StepForward();

}

void MazeWindow::AutoStepBackButtonClicked(){

    if (!bAutoState){
        StartAutoState(false);
    }
    if (bAutoPlay){
        AutoPause();
    }
    StepBack();

}

void MazeWindow::StartPointButtonClicked(){
    ChangeToStartPointMode();
}

void MazeWindow::FinishPointButtonClicked(){
    ChangeToFinishPointMode();
}

void MazeWindow::BlockPointButtonClicked(){
    ChangeToBlockPointMode();
}

void MazeWindow::ClearPointButtonClicked(){
    ChangeToClearPointMode();
}

void MazeWindow::EditCompleteButtonClicked(){
    EndAutoState();
    bool bMapValid = EditComplete();
    if (!bMapValid){
        QMessageBox::warning(this, "Map is invalid", "a valid map need at one start point and one finsh point!");
    }
}

void MazeWindow::ViewMapButtonPress(){
    UpdateSpecialMap();
}

void MazeWindow::ViewMapButtonRelease(){
    UpdateMap();
}

void MazeWindow::InitializeConnection(){
    connect(ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClick()));
    connect(this, SIGNAL(OnBackButtonClick()), this, SLOT(CloseSelfWindow()));

    connect(ui->Button_PlayPause, SIGNAL(clicked(bool)), this, SLOT(AutoPlayPauseButtonClicked()));
    connect(ui->Button_StepForward, SIGNAL(clicked(bool)), this, SLOT(AutoStepForwardButtonClicked()));
    connect(ui->Button_StepBack, SIGNAL(clicked(bool)), this, SLOT(AutoStepBackButtonClicked()));
    connect(ui->Button_AutoStop, SIGNAL(clicked(bool)), this, SLOT(AutoStopButtonClicked()));

    connect(ui->Button_StartPoint, SIGNAL(clicked(bool)), this, SLOT(StartPointButtonClicked()));
    connect(ui->Button_FinishPoint, SIGNAL(clicked(bool)), this, SLOT(FinishPointButtonClicked()));
    connect(ui->Button_ObstaclePoint, SIGNAL(clicked(bool)), this, SLOT(BlockPointButtonClicked()));
    connect(ui->Button_ClearPoint, SIGNAL(clicked(bool)), this, SLOT(ClearPointButtonClicked()));
    connect(ui->Button_CompleteEdit, SIGNAL(clicked(bool)), this, SLOT(EditCompleteButtonClicked()));

    connect(ui->Button_ViewMap, SIGNAL(pressed()), this, SLOT(ViewMapButtonPress()));
    connect(ui->Button_ViewMap, SIGNAL(released()), this, SLOT(ViewMapButtonRelease()));
}

void MazeWindow::InitializeUI(){
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Play");
    }
    if (ui->Content_Maze != nullptr){
        ContentLayout = new QGridLayout(ui->Content_Maze);
        ContentLayout->setSpacing(1);
        for (int32_t Row = 0; Row < MapSize; Row++){
            for (int32_t Col = 0; Col < MapSize; Col++){
                QLabel* ChildLabel = new QLabel(ui->Content_Maze);
                QPalette DefaultLabelColor(QPalette::Background, DefaultMapUnitColor);
                ChildLabel->setAutoFillBackground(true);
                ChildLabel->setPalette(DefaultLabelColor);
                ChildLabel->installEventFilter(this);
                ChildLabel->raise();
                ChildLabel->show();
                ContentLayout->addWidget(ChildLabel, Row, Col);
            }
        }
    }
}

void MazeWindow::ClearAllDynamic(){
    Core.ClearAllDynamic();
    UpdateMap();
    bEditComplete = false;
}

void MazeWindow::ClearAll(){

}

void MazeWindow::UpdateMap(){
    if (ContentLayout == nullptr){
        return;
    }
    int32_t CurrentPointRow = -1;
    int32_t CurrentPointCol = -1;
    Core.GetCurrentPointPosition(CurrentPointRow, CurrentPointCol);
    for (int32_t Row = 0; Row < MapSize; Row++){
        for (int32_t Col = 0; Col < MapSize; Col++){
            if (Row == CurrentPointRow && Col == CurrentPointCol){
                continue;
            }
            QLayoutItem* LabelItem = ContentLayout->itemAtPosition(Row, Col);
            if (LabelItem != nullptr){
                QLabel* Label = qobject_cast<QLabel*>(LabelItem->widget());
                if (Label != nullptr){
                    Maze::MazeUnitType LabelUnitType = Core.GetUnitType(Row, Col);
                    ChangeLabelToDefaultColorByUnitType(Label, LabelUnitType);
                    if (LabelUnitType == Maze::MazeUnitType::EOpen){
                        Label->setText(QString());
                    }
                }
            }
        }
    }
}

void MazeWindow::UpdateSpecialMap(){
    if (ContentLayout == nullptr){
        return;
    }
    int32_t CurrentPointRow = -1;
    int32_t CurrentPointCol = -1;
    Core.GetCurrentPointPosition(CurrentPointRow, CurrentPointCol);
    for (int32_t Row = 0; Row < MapSize; Row++){
        for (int32_t Col = 0; Col < MapSize; Col++){
            if (Row == CurrentPointRow && Col == CurrentPointCol){
                continue;
            }
            QLayoutItem* LabelItem = ContentLayout->itemAtPosition(Row, Col);
            if (LabelItem != nullptr){
                QLabel* Label = qobject_cast<QLabel*>(LabelItem->widget());
                if (Label != nullptr){
                    Maze::MazeUnitType LabelUnitType = Core.GetUnitType(Row, Col);
                    ChangeLabelToSpecialColorByUnitType(Label, LabelUnitType);
                    if (LabelUnitType == Maze::MazeUnitType::EOpen){
                        float Value = Core.GetUnitValue(Row, Col);
                        Label->setText(QString::number(Value));
                    }
                }
            }
        }
    }
}

void MazeWindow::StartAutoState(bool bInAutoPlay){

    if (bAutoState || bAutoPlay){
        return;
    }
    bAutoState = true;
    if (bInAutoPlay){
        StartAutoPlay();
    }

}

void MazeWindow::EndAutoState(){

    if (!bAutoState){
        return;
    }
    if (bAutoPlay){
        EndAutoPlay();
    }
    else{
        if (ui->Button_PlayPause != nullptr){
            ui->Button_PlayPause->setText("Play");
        }
    }
    ClearAllDynamic();
    bAutoState = false;

}

void MazeWindow::StartAutoPlay(){

    if (!bAutoState || bAutoPlay){
        return;
    }
    if (AutoPlayerTimer == nullptr){
        AutoPlayerTimer = new QTimer(this);
        AutoPlayerTimer->setInterval(AutoPlayIntervalTime);
        connect(AutoPlayerTimer, &QTimer::timeout, this, &MazeWindow::AutoStepForward);
    }
    AutoPlayerTimer->start();
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Pause");
    }
    bAutoPlay = true;

}

void MazeWindow::EndAutoPlay(){

    if (!bAutoState || !bAutoPlay){
        return;
    }
    AutoPlayerTimer->stop();
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Play");
    }
    bAutoPlay = false;

}

void MazeWindow::AutoPause(){

    if (!bAutoState || !bAutoPlay){
        return;
    }
    if (AutoPlayerTimer == nullptr){
        return;
    }
    AutoPlayerTimer->stop();
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Continue");
    }
    bAutoPlay = false;

}

void MazeWindow::StepForward(){
    if (!bEditComplete){
        bool bValidMap = EditComplete();
        if (!bValidMap){
            QMessageBox::warning(this, "Map is invalid", "a valid map need at one start point and one finsh point!");
            EndAutoState();
            return;
        }
    }
    std::vector<std::shared_ptr<Maze::Process>> StepForwardProcessArray = Core.StepForward();
    for (std::shared_ptr<Maze::Process> StepForwardProcess : StepForwardProcessArray){
        if (StepForwardProcess->Class == Maze::ProcessClass::EMove){
            std::shared_ptr<Maze::MoveProcess> MoveProcess = std::static_pointer_cast<Maze::MoveProcess>(StepForwardProcess);
            if (MoveProcess){
                if (ContentLayout != nullptr){
                    Maze::MazeUnitType OldType = Core.GetUnitType(MoveProcess->FromRow, MoveProcess->FromCol);
                    QLayoutItem* OldCurrentLabelItem = ContentLayout->itemAtPosition(MoveProcess->FromRow, MoveProcess->FromCol);
                    if (OldCurrentLabelItem != nullptr){
                        QLabel* OldCurrentLabel = qobject_cast<QLabel*>(OldCurrentLabelItem->widget());
                        if (OldCurrentLabel != nullptr){
                            ChangeLabelToDefaultColorByUnitType(OldCurrentLabel, OldType);
                        }
                    }
                    QLayoutItem* NewCurrentLabelItem = ContentLayout->itemAtPosition(MoveProcess->ToRow, MoveProcess->ToCol);
                    if (NewCurrentLabelItem != nullptr){
                        QLabel* NewCurrentLabel = qobject_cast<QLabel*>(NewCurrentLabelItem->widget());
                        if (NewCurrentLabel != nullptr){
                            QPalette NormalUnitColor(QPalette::Background, CurrentMapUnitColor);
                            NewCurrentLabel->setAutoFillBackground(true);
                            NewCurrentLabel->setPalette(NormalUnitColor);
                        }
                    }
                }
            }
        }
    }
    if (Core.IsSuccessful()){
        OnSuccess();
    }
    if (Core.IsFail()){
        OnFail();
    }
}

void MazeWindow::StepBack(){
    if (!bEditComplete){
        return;
    }
    std::vector<std::shared_ptr<Maze::Process>> StepBackProcessArray = Core.StepBack();
    for (std::shared_ptr<Maze::Process> StepForwardProcess : StepBackProcessArray){
        if (StepForwardProcess->Class == Maze::ProcessClass::EMove){
            std::shared_ptr<Maze::MoveProcess> MoveProcess = std::static_pointer_cast<Maze::MoveProcess>(StepForwardProcess);
            if (MoveProcess){
                if (ContentLayout != nullptr){
                    Maze::MazeUnitType OldType = Core.GetUnitType(MoveProcess->FromRow, MoveProcess->FromCol);
                    QLayoutItem* OldCurrentLabelItem = ContentLayout->itemAtPosition(MoveProcess->FromRow, MoveProcess->FromCol);
                    if (OldCurrentLabelItem != nullptr){
                        QLabel* OldCurrentLabel = qobject_cast<QLabel*>(OldCurrentLabelItem->widget());
                        if (OldCurrentLabel != nullptr){
                            ChangeLabelToDefaultColorByUnitType(OldCurrentLabel, OldType);
                        }
                    }
                    QLayoutItem* NewCurrentLabelItem = ContentLayout->itemAtPosition(MoveProcess->ToRow, MoveProcess->ToCol);
                    if (NewCurrentLabelItem != nullptr){
                        QLabel* NewCurrentLabel = qobject_cast<QLabel*>(NewCurrentLabelItem->widget());
                        if (NewCurrentLabel != nullptr){
                            QPalette NormalUnitColor(QPalette::Background, CurrentMapUnitColor);
                            NewCurrentLabel->setAutoFillBackground(true);
                            NewCurrentLabel->setPalette(NormalUnitColor);
                        }
                    }
                }
            }
        }
    }
}

void MazeWindow::OnSuccess(){
    EndAutoPlay();
    UpdateMap();
    QMessageBox::information(this, QString("Success"), QString("Success!"));
}

void MazeWindow::OnFail(){
    EndAutoPlay();
    QMessageBox::information(this, QString("Fail"), QString("Fail!"));
}

void MazeWindow::ChangeToStartPointMode(){
    if (CurrentCursorMode == CursorMode::EStartPoint){
        CurrentCursorMode = CursorMode::ENone;
        setCursor(Qt::ArrowCursor);
        return;
    }
    CurrentCursorMode = CursorMode::EStartPoint;
    setCursor(Qt::PointingHandCursor);
}

void MazeWindow::ChangeToFinishPointMode(){
    if (CurrentCursorMode == CursorMode::EFinishPoint){
        CurrentCursorMode = CursorMode::ENone;
        setCursor(Qt::ArrowCursor);
        return;
    }
    CurrentCursorMode = CursorMode::EFinishPoint;
    setCursor(Qt::CrossCursor);
}

void MazeWindow::ChangeToBlockPointMode(){
    if (CurrentCursorMode == CursorMode::EBlockPoint){
        CurrentCursorMode = CursorMode::ENone;
        setCursor(Qt::ArrowCursor);
        return;
    }
    CurrentCursorMode = CursorMode::EBlockPoint;
    setCursor(Qt::ForbiddenCursor);
}

void MazeWindow::ChangeToClearPointMode(){
    if (CurrentCursorMode == CursorMode::EClearPoint){
        CurrentCursorMode = CursorMode::ENone;
        setCursor(Qt::ArrowCursor);
        return;
    }
    CurrentCursorMode = CursorMode::EClearPoint;
    setCursor(Qt::OpenHandCursor);
}

void MazeWindow::ProcessChangeMapUnit(const int32_t& InRow, const int32_t& InCol){
    if (CurrentCursorMode == CursorMode::ENone){
        return;
    }
    if (bAutoState){
        EndAutoState();
    }
    if (ContentLayout == nullptr){
        return;
    }
    QLabel* UnitLabel = nullptr;
    QLayoutItem* UnitLabelItem = ContentLayout->itemAtPosition(InRow, InCol);
    if (UnitLabelItem != nullptr){
        UnitLabel = qobject_cast<QLabel*>(UnitLabelItem->widget());
    }
    if (UnitLabel == nullptr){
        return;
    }
    if (CurrentCursorMode == CursorMode::EStartPoint){
        int32_t OldStartPointRow = -1;
        int32_t OldStartPointCol = -1;
        if (Core.GetStartPointPosition(OldStartPointRow, OldStartPointCol)){
            QLabel* OldStartPointLabel = nullptr;
            QLayoutItem* OldStartPointLabelItem = ContentLayout->itemAtPosition(OldStartPointRow, OldStartPointCol);
            if (OldStartPointLabelItem != nullptr){
                OldStartPointLabel = qobject_cast<QLabel*>(OldStartPointLabelItem->widget());
            }
            if (OldStartPointLabel != nullptr){
                QPalette NormalPointColor(QPalette::Background, DefaultMapUnitColor);
                OldStartPointLabel->setAutoFillBackground(true);
                OldStartPointLabel->setPalette(NormalPointColor);
            }
        }
        Core.SetStartPoint(InRow, InCol);
        QPalette StartPointColor(QPalette::Background, StartPointMapUnitColor);
        UnitLabel->setAutoFillBackground(true);
        UnitLabel->setPalette(StartPointColor);
    }
    else if (CurrentCursorMode == CursorMode::EFinishPoint){
        int32_t OldFinishPointRow = -1;
        int32_t OldFinishPointCol = -1;
        if (Core.GetFinishPointPosition(OldFinishPointRow, OldFinishPointCol)){
            QLabel* OldFinishPointLabel = nullptr;
            QLayoutItem* OldFinishPointLabelItem = ContentLayout->itemAtPosition(OldFinishPointRow, OldFinishPointCol);
            if (OldFinishPointLabelItem != nullptr){
                OldFinishPointLabel = qobject_cast<QLabel*>(OldFinishPointLabelItem->widget());
            }
            if (OldFinishPointLabel != nullptr){
                QPalette NormalPointColor(QPalette::Background, DefaultMapUnitColor);
                OldFinishPointLabel->setAutoFillBackground(true);
                OldFinishPointLabel->setPalette(NormalPointColor);
            }
        }
        Core.SetFinishPoint(InRow, InCol);
        QPalette FinishPointColor(QPalette::Background, FinishPointMapUnitColor);
        UnitLabel->setAutoFillBackground(true);
        UnitLabel->setPalette(FinishPointColor);
    }
    else if (CurrentCursorMode == CursorMode::EBlockPoint){
        Core.SetBlockPoint(InRow, InCol);
        QPalette BlockPointColor(QPalette::Background, BlockMapUnitColor);
        UnitLabel->setAutoFillBackground(true);
        UnitLabel->setPalette(BlockPointColor);
    }
    else if (CurrentCursorMode == CursorMode::EClearPoint){
        Core.ClearPoint(InRow, InCol);
        QPalette NormalUnitColor(QPalette::Background, DefaultMapUnitColor);
        UnitLabel->setAutoFillBackground(true);
        UnitLabel->setPalette(NormalUnitColor);
    }
}

void MazeWindow::ChangeLabelToDefaultColorByUnitType(QLabel* InLabel, Maze::MazeUnitType InUnitType){
    if (InLabel == nullptr){
        return;
    }
    if (InUnitType == Maze::MazeUnitType::ENone){
        QPalette HoverLeaveColor(QPalette::Background, DefaultMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverLeaveColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EStartPoint){
        QPalette HoverLeaveColor(QPalette::Background, StartPointMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverLeaveColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EFinishPoint){
        QPalette HoverLeaveColor(QPalette::Background, FinishPointMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverLeaveColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EBlock){
        QPalette HoverLeaveColor(QPalette::Background, BlockMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverLeaveColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EOpen){
        QPalette HoverEnterColor(QPalette::Background, DefaultMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EClose){
        QPalette HoverEnterColor(QPalette::Background, DefaultMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EPath){
        QPalette HoverEnterColor(QPalette::Background, PathMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
}

void MazeWindow::ChangeLabelToHoverEnterColorByUnitType(QLabel* InLabel, Maze::MazeUnitType InUnitType){
    if (InLabel == nullptr){
        return;
    }
    if (InUnitType == Maze::MazeUnitType::ENone){
        QPalette HoverEnterColor(QPalette::Background, HoverEnterDefaultMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EStartPoint){
        QPalette HoverEnterColor(QPalette::Background, HoverEnterStartPointMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EFinishPoint){
        QPalette HoverEnterColor(QPalette::Background, HoverEnterFinishPointMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EBlock){
        QPalette HoverEnterColor(QPalette::Background, HoverEnterBlockMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EOpen){
        QPalette HoverEnterColor(QPalette::Background, HoverEnterDefaultMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EClose){
        QPalette HoverEnterColor(QPalette::Background, HoverEnterDefaultMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
}

void MazeWindow::ChangeLabelToSpecialColorByUnitType(QLabel* InLabel, Maze::MazeUnitType InUnitType){
    if (InLabel == nullptr){
        return;
    }
    if (InUnitType == Maze::MazeUnitType::ENone){
        QPalette HoverEnterColor(QPalette::Background, DefaultMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EStartPoint){
        QPalette HoverEnterColor(QPalette::Background, StartPointMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EFinishPoint){
        QPalette HoverEnterColor(QPalette::Background, FinishPointMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EBlock){
        QPalette HoverEnterColor(QPalette::Background, BlockMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EOpen){
        QPalette HoverEnterColor(QPalette::Background, OpenMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
    else if (InUnitType == Maze::MazeUnitType::EClose){
        QPalette HoverEnterColor(QPalette::Background, CloseMapUnitColor);
        InLabel->setAutoFillBackground(true);
        InLabel->setPalette(HoverEnterColor);
    }
}

bool MazeWindow::EditComplete(){
    int32_t FinishPointRow = -1;
    int32_t FinishPointCol = -1;
    int32_t StartPointRow = -1;
    int32_t StartPointCol = -1;
    if (!Core.GetStartPointPosition(StartPointRow, StartPointCol) || !Core.GetFinishPointPosition(FinishPointRow, FinishPointCol)){
        return false;
    }
    std::vector<std::shared_ptr<Maze::Process>> OutProcessArray;
    Core.EditComplete(OutProcessArray);
    if (ContentLayout != nullptr){
        QLayoutItem* StartPointLabelItem = ContentLayout->itemAtPosition(StartPointRow, StartPointCol);
        if (StartPointLabelItem != nullptr){
            QLabel* StartPointLabel = qobject_cast<QLabel*>(StartPointLabelItem->widget());
            if (StartPointLabel != nullptr){
                QPalette NormalUnitColor(QPalette::Background, CurrentMapUnitColor);
                StartPointLabel->setAutoFillBackground(true);
                StartPointLabel->setPalette(NormalUnitColor);
            }
        }
    }
    bEditComplete = true;
    CurrentCursorMode = CursorMode::ENone;
    setCursor(Qt::ArrowCursor);
    return true;
}


