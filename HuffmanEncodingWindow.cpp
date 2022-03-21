#include "HuffmanEncodingWindow.h"
#include "ui_HuffmanEncodingWindow.h"

#include "qdebug.h"

HuffmanEncodingWindow::HuffmanEncodingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HuffmanEncodingWindow)
{
    ui->setupUi(this);
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

void HuffmanEncodingWindow::InitializeConnection(){
    connect(ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
    connect(this, SIGNAL(OnBackButtonClicked()), this, SLOT(CloseSelf()));
}
