#include "EightQueenWindow.h"
#include "ui_EightQueenWindow.h"

EightQueenWindow::EightQueenWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EightQueenWindow)
{
    ui->setupUi(this);
    InitializeConnection();
    InitializeMap();
    // TODO: UnitLabel->installEventFilter;
}

EightQueenWindow::~EightQueenWindow()
{
    qDebug("[EightQueenWindow::~EightQueenWindow()] EightQueen UI has been destroyed.");
    delete ui;
}

bool EightQueenWindow::eventFilter(QObject *watched, QEvent *event){
    return false;
}

void EightQueenWindow::InitializeConnection(){
    connect(this->ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
    connect(this, SIGNAL(OnBackButtonClicked()), this, SLOT(CloseSelf()));
}

void EightQueenWindow::InitializeMap(){
    if (ui->EightQueenMap != nullptr){
        for (int Row = 0; Row < MapSize; Row++){
            QLayoutItem* MapRowLayOutItem = ui->EightQueenMap->itemAt(Row);
            if (MapRowLayOutItem != nullptr){
                QHBoxLayout* MapRowLayOut = qobject_cast<QHBoxLayout*>(MapRowLayOutItem->layout());
                if (MapRowLayOut != nullptr){
                    for (int Col = 0; Col < MapSize; Col++){
                        QLayoutItem* UnitLayoutItem = MapRowLayOut->itemAt(Col);
                        if (UnitLayoutItem != nullptr){
                             QLabel* UnitLabel = qobject_cast<QLabel*>(UnitLayoutItem->widget());
                             if (UnitLabel != nullptr){
                                 // TODO: Put labels in a structure.
                                 const bool bBlack = ((Row & 1) ^ (Col & 1));
                                 if (bBlack){
                                    UnitLabel->setStyleSheet("QLabel{background:#000000;}");
                                 }
                                 else{
                                    UnitLabel->setStyleSheet("QLabel{background:#ffffff;}");
                                 }
                             }
                        }
                    }
                }
            }
        }
    }
}

void EightQueenWindow::BackButtonClicked(){
    emit(OnBackButtonClicked());
}

void EightQueenWindow::CloseSelf(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}
