#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MazeWindow.h"
#include "EightQueenWindow.h"
#include "HuffmanEncodingWindow.h"

#include "QDebug"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitializeConnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitializeConnection(){
    connect(ui->Button_Maze, SIGNAL(clicked(bool)), this, SLOT(MazeButtonClick()));
    connect(this, SIGNAL(OnMazeButtonClick()), this, SLOT(OpenMazeUI()));
    connect(ui->Button_EightQueen, SIGNAL(clicked(bool)), this, SLOT(EightQueenButtonClick()));
    connect(this, SIGNAL(OnEightQueenButtonClick()), this, SLOT(OpenEightQueenUI()));
    connect(ui->Button_Huffman, SIGNAL(clicked(bool)), this, SLOT(HuffmanEncodingButtonClick()));
    connect(this, SIGNAL(OnHuffmanEncodingButtonClick()), this, SLOT(OpenHuffmanEncodingUI()));
}

void MainWindow::MazeButtonClick(){
    emit(OnMazeButtonClick());
}

void MainWindow::EightQueenButtonClick(){
    emit(OnEightQueenButtonClick());
}

void MainWindow::HuffmanEncodingButtonClick(){
    emit(OnHuffmanEncodingButtonClick());
}

void MainWindow::ShowMainWindow(){
    this->setVisible(true);
}

void MainWindow::OpenMazeUI(){
    MazeWindow* MazeUI = new MazeWindow();
    if (MazeUI != nullptr){
        this->setVisible(false);
        connect(MazeUI, SIGNAL(OnBackButtonClick()), this, SLOT(ShowMainWindow()));
        MazeUI->show();
    }
}

void MainWindow::OpenEightQueenUI(){
    EightQueenWindow* EightQueenUI = new EightQueenWindow();
    if (EightQueenUI != nullptr){
        this->setVisible(false);
        connect(EightQueenUI, SIGNAL(OnBackButtonClicked()), this, SLOT(ShowMainWindow()));
        EightQueenUI->show();
    }
}

void MainWindow::OpenHuffmanEncodingUI(){
    HuffmanEncodingWindow* HuffmanEncodingUI = new HuffmanEncodingWindow();
    if (HuffmanEncodingUI != nullptr){
        this->setVisible(false);
        connect(HuffmanEncodingUI, SIGNAL(OnBackButtonClicked()), this, SLOT(ShowMainWindow()));
        HuffmanEncodingUI->show();
    }
}

