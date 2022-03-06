#include "EightQueenWindow.h"
#include "ui_EightQueenWindow.h"

EightQueenWindow::EightQueenWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EightQueenWindow)
{
    ui->setupUi(this);
    connect(this->ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
    connect(this, SIGNAL(OnBackButtonClicked()), this, SLOT(CloseSelf()));
    ui->EightQueenUnit_0->setStyleSheet("QLabel{background:#000000;}");
}

EightQueenWindow::~EightQueenWindow()
{
    qDebug("[EightQueenWindow::~EightQueenWindow()] EightQueen UI has been destroyed.");
    delete ui;
}

void EightQueenWindow::BackButtonClicked(){
    emit(OnBackButtonClicked());
}

void EightQueenWindow::CloseSelf(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}
