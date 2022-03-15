#include "EightQueenWindow.h"
#include "ui_EightQueenWindow.h"

#include "QDebug"
#include "QMouseEvent"
#include "QPropertyAnimation"
#include "qparallelanimationgroup.h"
#include "qsequentialanimationgroup.h"
#include "QElapsedTimer"
#include "QGraphicsOpacityEffect"

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

void EightQueenWindow::RemoveChessButtonClicked(){
    TryRemoveChess();
}

void EightQueenWindow::ViewMapButtonPressed(){
    ViewMap();
}

void EightQueenWindow::ViewMapButtonReleased(){
    RecoverMap();
}

void EightQueenWindow::CloseSelf(){
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->close();
}

void EightQueenWindow::InitializeConnection(){
    connect(this->ui->Button_Back, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
    connect(this, SIGNAL(OnBackButtonClicked()), this, SLOT(CloseSelf()));
    connect(this->ui->Button_RemoveChess, SIGNAL(clicked(bool)), this, SLOT(RemoveChessButtonClicked()));
    connect(this->ui->Button_ViewMap, SIGNAL(pressed()), this, SLOT(ViewMapButtonPressed()));
    connect(this->ui->Button_ViewMap, SIGNAL(released()), this, SLOT(ViewMapButtonReleased()));
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
                    QLabel* NewChessLabel = new QLabel(ui->layoutWidget);
                    NewChessLabel->setGeometry(UnitLabel->pos().x(),
                                          UnitLabel->pos().y(),
                                          UnitLabel->geometry().width(),
                                          UnitLabel->geometry().height());
                    QPixmap QueenChessPixmap(QString(":/EightQueen/QueenChess.jpg"));
                    QueenChessPixmap.scaled(NewChessLabel->size());
                    NewChessLabel->setScaledContents(true);
                    NewChessLabel->setPixmap(QueenChessPixmap);
                    NewChessLabel->show();
                    ChessStack.push(NewChessLabel);
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

void EightQueenWindow::TryRemoveChess(){
    if (Core.TryReduceQueenChess()){
        QLabel* RemoveChess = ChessStack.top();
        if (RemoveChess != nullptr){
           const QRect& OriginalGeometry = RemoveChess->geometry();
           const QRect MagnifyingGeometry = QRect(
                       OriginalGeometry.x() - OriginalGeometry.width() * 0.2f / 2.0f,
                       OriginalGeometry.y() - OriginalGeometry.height() * 0.2f / 2.0f,
                       OriginalGeometry.width() * 1.2f,
                       OriginalGeometry.height() * 1.2f);
           const QRect ShrinkingGeometry = QRect(
                       OriginalGeometry.x() + OriginalGeometry.width() * 0.2f / 2.0f,
                       OriginalGeometry.y() + OriginalGeometry.height() * 0.2f / 2.0f,
                       OriginalGeometry.width() * 0.8f,
                       OriginalGeometry.height() * 0.8f);
           QParallelAnimationGroup* ParallelGroup = new QParallelAnimationGroup(RemoveChess);
           QSequentialAnimationGroup* ScaleGroup = new QSequentialAnimationGroup(RemoveChess);
           QPropertyAnimation* MagnifyingAnimation = new QPropertyAnimation(RemoveChess, "geometry");
           MagnifyingAnimation->setStartValue(OriginalGeometry);
           MagnifyingAnimation->setEndValue(MagnifyingGeometry);
           MagnifyingAnimation->setDuration(500);
           ScaleGroup->addAnimation(MagnifyingAnimation);
           QPropertyAnimation* ShrinkingAnimation = new QPropertyAnimation(RemoveChess, "geometry");
           ShrinkingAnimation->setStartValue(MagnifyingGeometry);
           ShrinkingAnimation->setEndValue(ShrinkingGeometry);
           ShrinkingAnimation->setDuration(500);
           ScaleGroup->addAnimation(ShrinkingAnimation);
           ParallelGroup->addAnimation(ScaleGroup);
           QGraphicsOpacityEffect* LabelOpacity = new QGraphicsOpacityEffect(RemoveChess);
           LabelOpacity->setOpacity(1.0f);
           RemoveChess->setGraphicsEffect(LabelOpacity);
           if (LabelOpacity != nullptr){
               QPropertyAnimation* OpacityGroup = new QPropertyAnimation(LabelOpacity, "opacity");
               OpacityGroup->setStartValue(1.0f);
               OpacityGroup->setEndValue(0.0f);
               OpacityGroup->setDuration(1000);
               ParallelGroup->addAnimation(OpacityGroup);
           }
           ParallelGroup->start(QAbstractAnimation::DeleteWhenStopped);
           connect(ParallelGroup, &QAbstractAnimation::finished, [RemoveChess]{
                RemoveChess->deleteLater();
           });
           ChessStack.pop();
        }
    }
    else{
        qDebug() << "Remove Chess Fail !!";
    }
}

void EightQueenWindow::ViewMap(){
    if (!MemoryMap.empty()){
        return;
    }
    for (uint8_t Row = 0; Row < MapSize; Row++){
        for (uint8_t Col = 0; Col < MapSize; Col++){
            if (!Core.IsPositionValid(Row, Col)){
                if (ui->GridLayout_EightQueen != nullptr){
                    QLayoutItem* UnitItem = ui->GridLayout_EightQueen->itemAtPosition(Row, Col);
                    if (UnitItem != nullptr){
                        QLabel* InvalidLabel = qobject_cast<QLabel*>(UnitItem->widget());
                        if (InvalidLabel != nullptr){
                            InvalidLabel->setStyleSheet("QLabel{background:#ff0000;}");
                            bool bIsBlack = ((Row & 1) ^ (Col & 1));
                            MemoryMap.insert(InvalidLabel, bIsBlack);
                        }
                    }
                }
            }
        }
    }
}

void EightQueenWindow::RecoverMap(){
    for (auto Iter = MemoryMap.begin(); Iter != MemoryMap.end(); Iter++){
        if (Iter.key() != nullptr){
            if (Iter.value()){
                Iter.key()->setStyleSheet("QLabel{background:#000000}");
            }
            else{
                Iter.key()->setStyleSheet("QLabel{background:#ffffff}");
            }
        }
    }
    MemoryMap.clear();
}

void EightQueenWindow::StepForward(){

}

void EightQueenWindow::OnSuccess(){

}

void EightQueenWindow::OnFail(){

}

