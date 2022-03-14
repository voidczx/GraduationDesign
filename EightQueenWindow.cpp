#include "EightQueenWindow.h"
#include "ui_EightQueenWindow.h"

#include "QDebug"
#include "QMouseEvent"
#include "QPropertyAnimation"
#include "QElapsedTimer"

const uint8_t EightQueenWindow::MapSize = 8;
const int32_t EightQueenWindow::AddChessFailWarningTime = 500;

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
                    if (ui->GridLayout_EightQueen != nullptr){
                        int Index = ui->GridLayout_EightQueen->indexOf(EventLabel);
                        uint8_t Row = Index / MapSize;
                        uint8_t Col = Index % MapSize;
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
    emit(OnBackButtonClicked());
}

void EightQueenWindow::CloseSelf(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}

void EightQueenWindow::InitializeConnection(){
    connect(this->ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
    connect(this, SIGNAL(OnBackButtonClicked()), this, SLOT(CloseSelf()));
}

void EightQueenWindow::InitializeMap(){
    if (ui->GridLayout_EightQueen != nullptr){
        for (int Row = 0; Row < MapSize; Row++){
            for (int Col = 0; Col < MapSize; Col++){
                QLayoutItem* UnitItem = ui->GridLayout_EightQueen->itemAtPosition(Row, Col);
                if (UnitItem != nullptr){
                    QLabel* UnitLabel = qobject_cast<QLabel*>(UnitItem->widget());
                    if (UnitLabel != nullptr){
                        const bool bBlack = ((Row & 1) ^ (Col & 1));
                        if (bBlack){
                            UnitLabel->setStyleSheet("QLabel{background:#000000;}");
                        }
                        else{
                            UnitLabel->setStyleSheet("QLabel{background:#ffffff;}");
                        }
                        UnitLabel->installEventFilter(this);
                    }
                }
            }
        }
    }
}

void EightQueenWindow::TryAddChess(const uint8_t& Row, const uint8_t& Col){
    if (Core.TryAddQueenChess(Row, Col)){
        qDebug() << "Add Success At " << Row << ", " << Col;
        if (ui->GridLayout_EightQueen != nullptr){
            QLayoutItem* UnitItem = ui->GridLayout_EightQueen->itemAtPosition(Row, Col);
            if (UnitItem != nullptr){
                QLabel* UnitLabel = qobject_cast<QLabel*>(UnitItem->widget());
                if (UnitLabel != nullptr){
                    QLabel* NewChessLabel = new QLabel(ui->widget);
                    NewChessLabel->setGeometry(UnitLabel->pos().x(),
                                          UnitLabel->pos().y(),
                                          UnitLabel->geometry().width(),
                                          UnitLabel->geometry().height());
                    QPixmap QueenChessPixmap(QString(":/EightQueen/QueenChess.jpg"));
                    QueenChessPixmap.scaled(NewChessLabel->size());
                    NewChessLabel->setScaledContents(true);
                    NewChessLabel->setPixmap(QueenChessPixmap);
                    NewChessLabel->show();
                }
            }
        }
        if (Core.IsSuccess()){
            OnSuccess();
        }
        else if (Core.IsFail()){
            OnFail();
        }
    }
    else{
        qDebug() << "Add Fail At " << Row << ", " << Col;
        if (ui->GridLayout_EightQueen != nullptr){
            QLayoutItem* UnitItem = ui->GridLayout_EightQueen->itemAtPosition(Row, Col);
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
}

void EightQueenWindow::StepForward(){

}

void EightQueenWindow::OnAddChessFail(const uint8_t& Row, const uint8_t& Col){

}

void EightQueenWindow::OnSuccess(){

}

void EightQueenWindow::OnFail(){

}

