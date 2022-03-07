#include "MazeWindow.h"
#include "ui_MazeWindow.h"

#include "QAnimationDriver"

MazeWindow::MazeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MazeWindow)
{
    ui->setupUi(this);
    InitializeConnection();
}

MazeWindow::~MazeWindow()
{
    qDebug("[MazeWindow::~MazeWindow()] Maze UI has been destroyed.");
    delete ui;
}

void MazeWindow::InitializeConnection(){
    connect(ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClick()));
    connect(this, SIGNAL(OnBackButtonClick()), this, SLOT(CloseSelfWindow()));
}

void MazeWindow::CloseSelfWindow(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}

void MazeWindow::BackButtonClick(){
    emit(OnBackButtonClick());
}
