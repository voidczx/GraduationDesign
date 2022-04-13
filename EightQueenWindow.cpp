#include "EightQueenWindow.h"
#include "ui_EightQueenWindow.h"

#include "QDebug"
#include "QMouseEvent"
#include "QPropertyAnimation"
#include "qparallelanimationgroup.h"
#include "qsequentialanimationgroup.h"
#include "QElapsedTimer"
#include "QGraphicsOpacityEffect"
#include "QMessageBox"

const uint8_t EightQueenWindow::MapSize = 8;
const int64_t EightQueenWindow::AddChessFailWarningTime = 500;
const int64_t EightQueenWindow::AutoPlaySpaceTime = 1500;
const int64_t EightQueenWindow::AutoViewMapTime = 500;

EightQueenWindow::EightQueenWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EightQueenWindow),
    Core(MapSize)
{
    ui->setupUi(this);
    InitializeConnection();
    InitializeMap();
    // TODO:
}

EightQueenWindow::~EightQueenWindow()
{
    qDebug("[EightQueenWindow::~EightQueenWindow()] EightQueen UI has been destroyed.");
    delete ui;
}

bool EightQueenWindow::eventFilter(QObject *watched, QEvent *event){
    if (QLabel* EventLabel = qobject_cast<QLabel*>(watched)){
        if (event->type() == QEvent::MouseButtonPress){
            if (QMouseEvent* MouseEvent = dynamic_cast<QMouseEvent*>(event)){
                if (MouseEvent->button() == Qt::LeftButton){
                    if (ContentLayout != nullptr){
                        int Index = ContentLayout->indexOf(EventLabel);
                        uint8_t Row = Index / MapSize;
                        uint8_t Col = Index % MapSize;
                        if (bAutoState){
                            EndAutoState();
                        }
                        TryAddChess(Row, Col);
                    }
                    return true;
                }
            }
        }
        return false;
    }
    return QWidget::eventFilter(watched, event);
}

void EightQueenWindow::BackButtonClicked(){
    if (bAutoState){
        EndAutoState();
    }
    emit(OnBackButtonClicked());
}

void EightQueenWindow::RemoveChessButtonClicked(){
    if (bAutoState){
        EndAutoState();
    }
    TryRemoveChess();
}

void EightQueenWindow::ViewMapButtonPressed(){
    if (bAutoPlay){
        AutoPause();
    }
    ViewMap();
}

void EightQueenWindow::ViewMapButtonReleased(){
    if (bAutoPlay){
        AutoPause();
    }
    RecoverMap();
}

void EightQueenWindow::AutoPlayPauseButtonClicked(){
    if (bAutoState){
        if (bAutoPlay){
            AutoPause();
        }
        else{
            AutoPlay();
        }
    }
    else{
        BeginAutoState(true);
    }
}

void EightQueenWindow::AutoStopButtonClicked(){
    EndAutoState();
}

void EightQueenWindow::AutoStepForwardClicked(){
    if (!bAutoState){
        BeginAutoState(false);
    }
    if (bAutoPlay){
        AutoPause();
    }
    StepForward();
}

void EightQueenWindow::AutoStepBackClicked(){
    if (!bAutoState){
        return;
    }
    if (bAutoPlay){
        AutoPause();
    }
    StepBack();
}

void EightQueenWindow::CloseSelf(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}

void EightQueenWindow::InitializeConnection(){
    connect(this->ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
    connect(this, SIGNAL(OnBackButtonClicked()), this, SLOT(CloseSelf()));
    connect(this->ui->Button_RemoveChess, SIGNAL(clicked(bool)), this, SLOT(RemoveChessButtonClicked()));
    connect(this->ui->Button_ViewMap, SIGNAL(pressed()), this, SLOT(ViewMapButtonPressed()));
    connect(this->ui->Button_ViewMap, SIGNAL(released()), this, SLOT(ViewMapButtonReleased()));
    connect(this->ui->Button_AutoPlayPause, SIGNAL(clicked(bool)), this, SLOT(AutoPlayPauseButtonClicked()));
    connect(this->ui->Button_AutoStop, SIGNAL(clicked(bool)), this, SLOT(AutoStopButtonClicked()));
    connect(this->ui->Button_AutoStepForward, SIGNAL(clicked(bool)), this, SLOT(AutoStepForwardClicked()));
    connect(this->ui->Button_AutoStepBack, SIGNAL(clicked(bool)), this, SLOT(AutoStepBackClicked()));
}

void EightQueenWindow::InitializeMap(){
    if (ContentLayout != nullptr){
        // Error
        return;
    }
    if (ui->Content_HuffMan == nullptr){
        return;
    }
    ContentLayout = new QGridLayout(ui->Content_HuffMan);
    if (ContentLayout != nullptr){
        ContentLayout->setSpacing(0);
        for (int Row = 0; Row < MapSize; Row++){
            for (int Col = 0; Col < MapSize; Col++){
                QLabel* NewLabel = new QLabel(ui->Content_HuffMan);
                if (NewLabel != nullptr){
                    NewLabel->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
                    ContentLayout->addWidget(NewLabel, Row, Col, 1, 1);
                    const bool bBlack = ((Row & 1) ^ (Col & 1));
                    if (bBlack){
                        NewLabel->setStyleSheet("QLabel{background:#000000;}");
                    }
                    else{
                        NewLabel->setStyleSheet("QLabel{background:#ffffff;}");
                    }
                    NewLabel->installEventFilter(this);
                    NewLabel->show();
                }
            }
        }
    }
    if (ui->Button_AutoPlayPause != nullptr){
        ui->Button_AutoPlayPause->setText("Play");
    }
}

void EightQueenWindow::TryAddChess(const uint8_t& Row, const uint8_t& Col){
    if (Core.TryAddQueenChess_Manually(Row, Col)){
        qDebug() << "Add Success At " << Row << ", " << Col;
        AfterAddChessSuccess(Row, Col);
        if (Core.IsSuccess()){
            OnSuccess();
        }
        else if (Core.IsFail()){
            OnFail();
        }
    }
    else{
        qDebug() << "Add Fail At " << Row << ", " << Col;
        AfterAddChessFail(Row, Col);
    }
}

void EightQueenWindow::TryRemoveChess(){
    if (Core.TryReduceQueenChess_Manually()){
        AfterReduceChess();
    }
    else{
        qDebug() << "Remove Chess Fail !!";
    }
}

void EightQueenWindow::AfterAddChessSuccess(const uint8_t& Row, const uint8_t& Col){
    if (ContentLayout != nullptr){
        QLayoutItem* UnitItem = ContentLayout->itemAtPosition(Row, Col);
        if (UnitItem != nullptr){
            QLabel* UnitLabel = qobject_cast<QLabel*>(UnitItem->widget());
            if (UnitLabel != nullptr){
                QSizePolicy OldSizePolicy = UnitLabel->sizePolicy();
                QPixmap QueenChessPixmap(QString(":/EightQueen/QueenChess.jpg"));
                QueenChessPixmap.scaled(UnitLabel->size());
                UnitLabel->setScaledContents(true);
                UnitLabel->setPixmap(QueenChessPixmap);
                ChessStack.push(UnitLabel);
            }
        }
    }
}

void EightQueenWindow::AfterAddChessFail(const uint8_t& Row, const uint8_t& Col){
    if (ContentLayout != nullptr){
        QLayoutItem* UnitItem = ContentLayout->itemAtPosition(Row, Col);
        if (UnitItem != nullptr){
            QLabel* RedLabel = qobject_cast<QLabel*>(UnitItem->widget());
            if (RedLabel != nullptr){
                RedLabel->setStyleSheet("QLabel{background:#ff0000;}");
                QElapsedTimer RecoverTimer;
                RecoverTimer.start();
                while (RecoverTimer.elapsed() < AddChessFailWarningTime){
                    QCoreApplication::processEvents();
                }
                const bool bBlack = ((Row & 1) ^ (Col & 1));
                if (bBlack){
                    RedLabel->setStyleSheet(QString("QLabel{background:#000000;}"));
                }
                else{
                    RedLabel->setStyleSheet(QString("QLabel{background:#ffffff;}"));
                }
            }
        }
    }
}

void EightQueenWindow::AfterReduceChess(){
    QLabel* RemoveChess = ChessStack.top();
    if (RemoveChess != nullptr){
       RemoveChess->clear();
       if (ContentLayout != nullptr){
           int32_t Index = ContentLayout->indexOf(RemoveChess);
           int32_t Row = Index / MapSize;
           int32_t Col = Index % MapSize;
           const bool bBlack = ((Row & 1) ^ (Col & 1));
           if (bBlack){
               RemoveChess->setStyleSheet(QString("QLabel{background:#000000;}"));
           }
           else{
               RemoveChess->setStyleSheet(QString("QLabel{background:#ffffff;}"));
           }
       }
       ChessStack.pop();
    }
}

void EightQueenWindow::ViewMap(){
    if (!MemoryMap.empty()){
        return;
    }
    for (uint8_t Row = 0; Row < MapSize; Row++){
        for (uint8_t Col = 0; Col < MapSize; Col++){
            if (!Core.IsPositionValid(Row, Col)){
                if (ContentLayout != nullptr){
                    QLayoutItem* UnitItem = ContentLayout->itemAtPosition(Row, Col);
                    if (UnitItem != nullptr){
                        QLabel* InvalidLabel = qobject_cast<QLabel*>(UnitItem->widget());
                        if (InvalidLabel != nullptr){
                            InvalidLabel->setStyleSheet("QLabel{background:#ff0000;}");
                            bool bIsBlack = ((Row & 1) ^ (Col & 1));
                            MemoryMap.insert(InvalidLabel, bIsBlack);
                        }
                    }
                }
            }
            else{
                if (bAutoState){
                    if (Row == Core.GetLastAutoReduceUnit().Row){
                        if (Col <= Core.GetLastAutoReduceUnit().Col){
                            if (ContentLayout != nullptr){
                                QLayoutItem* UnitItem = ContentLayout->itemAtPosition(Row, Col);
                                if (UnitItem != nullptr){
                                    QLabel* InvalidLabel = qobject_cast<QLabel*>(UnitItem->widget());
                                    if (InvalidLabel != nullptr){
                                        InvalidLabel->setStyleSheet("QLabel{background:#00ff00;}");
                                        bool bIsBlack = ((Row & 1) ^ (Col & 1));
                                        MemoryMap.insert(InvalidLabel, bIsBlack);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void EightQueenWindow::RecoverMap(){
    for (auto Iter = MemoryMap.begin(); Iter != MemoryMap.end(); Iter++){
        if (Iter.key() != nullptr){
            if (Iter.value()){
                Iter.key()->setStyleSheet("QLabel{background:#000000}");
            }
            else{
                Iter.key()->setStyleSheet("QLabel{background:#ffffff}");
            }
        }
    }
    MemoryMap.clear();
}

void EightQueenWindow::StepForward(){
    if (!bAutoState){
        return;
    }
    if (Core.IsFail() || Core.IsAutoFailed()){
        if (Core.TryReduceQueenChess_Auto()){
            AfterReduceChess();
        }
    }
    else{
        uint8_t Row = MapSize;
        uint8_t Col = MapSize;
        if (Core.TryAddQueenChess_Auto(Row, Col)){
            AfterAddChessSuccess(Row, Col);
        }
        else{
            AfterAddChessFail(Row, Col);
        }
    }
    if (Core.IsSuccess()){
        OnSuccess();
        return;
    }
    if (Core.IsAutoTotallyFailed()){
        OnFail();
        return;
    }
}

void EightQueenWindow::StepBack(){
    EightQueen::Process ShowProcess = Core.StepBack_Auto();
    if (ShowProcess.Type == EightQueen::ProcessType::EAdd){
        AfterAddChessSuccess(ShowProcess.OptionalInt1 / MapSize, ShowProcess.OptionalInt1 % MapSize);
    }
    else if (ShowProcess.Type == EightQueen::ProcessType::EReduce){
        AfterReduceChess();
    }
}

void EightQueenWindow::BeforeStepForward(){
    QElapsedTimer RecoverTimer;
    ViewMap();
    RecoverTimer.start();
    while (RecoverTimer.elapsed() < AutoViewMapTime){
        QCoreApplication::processEvents();
    }
    RecoverMap();
}

void EightQueenWindow::OnSuccess(){
    if (bAutoState){
        if (bAutoPlay){
            AutoPause();
        }
    }
    QMessageBox::information(this, QString("Success"), QString("Success!"));
}

void EightQueenWindow::OnFail(){
    if (bAutoState){
        if (bAutoPlay){
            AutoPause();
        }
    }
    QMessageBox::information(this, QString("Fail"), QString("Fail!"));
}

void EightQueenWindow::BeginAutoState(bool bInAutoPlay){
    if (bAutoState || bAutoPlay){
        return;
    }
    bAutoState = true;
    Core.InitializeAutoState();
    if (bInAutoPlay){
        AutoPlay();
    }
}

void EightQueenWindow::AutoPlay(){
    if (bAutoPlay || !bAutoState){
        return;
    }
    bAutoPlay = true;
    if (AutoPlayerTimer == nullptr){
        AutoPlayerTimer = new QTimer(this);
        AutoPlayerTimer->setInterval(AutoPlaySpaceTime);
        connect(AutoPlayerTimer, &QTimer::timeout, this, &EightQueenWindow::AutoStepForward);
    }
    AutoPlayerTimer->start();
    if (ui->Button_AutoPlayPause != nullptr){
        ui->Button_AutoPlayPause->setText("Pause");
    }
}

void EightQueenWindow::AutoStepForward(){
    BeforeStepForward();
    QElapsedTimer SpaceTimer;
    SpaceTimer.start();
    while (SpaceTimer.elapsed() <= AutoViewMapTime){
        QCoreApplication::processEvents();
    }
    StepForward();
}

void EightQueenWindow::AutoPause(){
    if (!bAutoPlay || !bAutoState){
        return;
    }
    bAutoPlay = false;
    if (AutoPlayerTimer == nullptr){
        return;
    }
    AutoPlayerTimer->stop();
    if (ui->Button_AutoPlayPause != nullptr){
        ui->Button_AutoPlayPause->setText("Continue");
    }
}

void EightQueenWindow::EndAutoState(){
    if (!bAutoState){
        return;
    }
    if (bAutoPlay){
        AutoPause();
    }
    bAutoState = false;
    Core.ClearAutoState();
    if (ui->Button_AutoPlayPause){
        ui->Button_AutoPlayPause->setText("Play");
    }
}

