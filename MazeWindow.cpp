#include "MazeWindow.h"
#include "ui_MazeWindow.h"

#include "QTimer"
#include "QGridLayout"
#include "QLabel"
#include "QDebug"

const QColor MazeWindow::DefaultMapUnitColor = QColor(0x9f9f9f);
const QColor MazeWindow::HoverEnterDefaultMapUnitColor = QColor(0xbfbfbf);

const int32_t MazeWindow::AutoPlayIntervalTime = 800;
const int32_t MazeWindow::MapSize = 19;

MazeWindow::MazeWindow(QWidget *parent) :
    QWidget(parent),
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
            QPalette HoverEnterColor(QPalette::Background, HoverEnterDefaultMapUnitColor);
            EventLabel->setAutoFillBackground(true);
            EventLabel->setPalette(HoverEnterColor);
            return true;
        }
        else if (event->type() == QEvent::Type::Leave){
            QPalette DefaultLabelColor(QPalette::Background, DefaultMapUnitColor);
            EventLabel->setAutoFillBackground(true);
            EventLabel->setPalette(DefaultLabelColor);
            return true;
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

void MazeWindow::InitializeConnection(){
    connect(ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClick()));
    connect(this, SIGNAL(OnBackButtonClick()), this, SLOT(CloseSelfWindow()));

    connect(ui->Button_PlayPause, SIGNAL(clicked(bool)), this, SLOT(AutoPlayPauseButtonClicked()));
    connect(ui->Button_StepForward, SIGNAL(clicked(bool)), this, SLOT(AutoStepForwardButtonClicked()));
    connect(ui->Button_StepBack, SIGNAL(clicked(bool)), this, SLOT(AutoStepBackButtonClicked()));
    connect(ui->Button_AutoStop, SIGNAL(clicked(bool)), this, SLOT(AutoStopButtonClicked()));
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

}

void MazeWindow::StepBack(){

}


