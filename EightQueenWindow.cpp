#include "EightQueenWindow.h"
#include "ui_EightQueenWindow.h"

#include "QDebug"
#include "QMouseEvent"

EightQueenWindow::EightQueenWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EightQueenWindow)
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
                    qDebug() << "Press Label Pos:" << "(" << EventLabel->parentWidget()->pos().x() + EventLabel->pos().x() << ", " << EventLabel->parentWidget()->pos().y() + EventLabel->pos().y() << ")";
                    QLabel TestLabel(this);

                    return true;
                }
            }
        }
        return false;
    }
    return QWidget::eventFilter(watched, event);
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

void EightQueenWindow::BackButtonClicked(){
    emit(OnBackButtonClicked());
}

void EightQueenWindow::CloseSelf(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}
