#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MazeWindow.h"

#include "QDebug"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->Button_Maze, SIGNAL(clicked(bool)), this, SLOT(MazeButtonClick()));
    connect(this, SIGNAL(OnMazeButtonClick()), this, SLOT(OpenMazeUI()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MazeButtonClick(){
    emit(OnMazeButtonClick());
}

void MainWindow::ShowMainWindow(){
    this->setVisible(true);
}

void MainWindow::OpenMazeUI(){
    MazeWindow* MazeUI = new MazeWindow();
    this->setVisible(false);
    connect(MazeUI, SIGNAL(OnBackButtonClick()), this, SLOT(ShowMainWindow()));
    MazeUI->show();
}

