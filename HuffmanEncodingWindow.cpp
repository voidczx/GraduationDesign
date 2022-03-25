#include "HuffmanEncodingWindow.h"
#include "ui_HuffmanEncodingWindow.h"

#include "QTimer"
#include "QPainter"

#include "qdebug.h"

int32_t HuffmanEncodingWindow::AutoPlayIntervalTime = 800;

HuffmanEncodingWindow::HuffmanEncodingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HuffmanEncodingWindow)
{
    ui->setupUi(this);
    InitializeUI();
    InitializeConnection();
}

HuffmanEncodingWindow::~HuffmanEncodingWindow()
{
    qDebug() << "HuffmanEncodingWindow has deleted!";
    delete ui;
}

void HuffmanEncodingWindow::BackButtonClicked(){
    emit(OnBackButtonClicked());
}

void HuffmanEncodingWindow::CloseSelf(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    close();
}

void HuffmanEncodingWindow::AutoStepForward(){
    StepForward();
}

void HuffmanEncodingWindow::AutoPlayPauseButtonClicked(){

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

void HuffmanEncodingWindow::AutoStopButtonClicked(){

    if (!bAutoState){
        return;
    }
    EndAutoState();

}

void HuffmanEncodingWindow::AutoStepForwardButtonClicked(){

    if (!bAutoState){
        StartAutoState(false);
    }
    if (bAutoPlay){
        AutoPause();
    }
    StepForward();

}

void HuffmanEncodingWindow::AutoStepBackButtonClicked(){

    if (!bAutoState){
        StartAutoState(false);
    }
    if (bAutoPlay){
        AutoPause();
    }
    StepBack();

}

void HuffmanEncodingWindow::InitializeConnection(){
    connect(ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
    connect(this, SIGNAL(OnBackButtonClicked()), this, SLOT(CloseSelf()));
    connect(ui->Button_PlayPause, SIGNAL(clicked(bool)), this, SLOT(AutoPlayPauseButtonClicked()));
    connect(ui->Button_StepForward, SIGNAL(clicked(bool)), this, SLOT(AutoStepForwardButtonClicked()));
    connect(ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(AutoStepBackButtonClicked()));
    connect(ui->Button_Stop, SIGNAL(clicked(bool)), this, SLOT(AutoStopButtonClicked()));
}

void HuffmanEncodingWindow::InitializeUI(){
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Play");
    }
    if (ui->ScrollArea_Huffman != nullptr){
        ui->ScrollArea_Huffman->viewport()->show();
    }
}

void HuffmanEncodingWindow::StartAutoState(bool bInAutoPlay){

    if (bAutoState || bAutoPlay){
        return;
    }
    bAutoState = true;
    if (bInAutoPlay){
        StartAutoPlay();
    }

}

void HuffmanEncodingWindow::EndAutoState(){

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

void HuffmanEncodingWindow::StartAutoPlay(){

    if (!bAutoState || bAutoPlay){
        return;
    }
    if (AutoPlayerTimer == nullptr){
        AutoPlayerTimer = new QTimer(this);
        AutoPlayerTimer->setInterval(AutoPlayIntervalTime);
        connect(AutoPlayerTimer, &QTimer::timeout, this, &HuffmanEncodingWindow::AutoStepForward);
    }
    AutoPlayerTimer->start();
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Pause");
    }
    bAutoPlay = true;

}

void HuffmanEncodingWindow::EndAutoPlay(){

    if (!bAutoState || !bAutoPlay){
        return;
    }
    AutoPlayerTimer->stop();
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Play");
    }
    bAutoPlay = false;

}

void HuffmanEncodingWindow::AutoPause(){

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

void HuffmanEncodingWindow::StepForward(){

    if (ui->Content_Huffman != nullptr){
        ui->Content_Huffman->update();
    }

}

void HuffmanEncodingWindow::StepBack(){

}


