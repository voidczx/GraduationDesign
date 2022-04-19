#include "HuffmanEncodingWindow.h"
#include "ui_HuffmanEncodingWindow.h"

#include "HuffmanEncodingViewWidget.h"

#include "QTimer"
#include "QPainter"
#include "QMessageBox"

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

void HuffmanEncodingWindow::InputWordsCompleted(){
    if (ui->LineEdit_OriginWords->text() == InputLine){
        return;
    }
    InputLine = ui->LineEdit_OriginWords->text();
    if (bAutoState){
        EndAutoState();
    }
    ClearAll();
    GenerateFrequencyMap();
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
    connect(ui->Button_StepBack, SIGNAL(clicked(bool)), this, SLOT(AutoStepBackButtonClicked()));
    connect(ui->Button_Stop, SIGNAL(clicked(bool)), this, SLOT(AutoStopButtonClicked()));
    connect(ui->LineEdit_OriginWords, SIGNAL(editingFinished()), this, SLOT(InputWordsCompleted()));
}

void HuffmanEncodingWindow::InitializeUI(){
    if (ui->Button_PlayPause != nullptr){
        ui->Button_PlayPause->setText("Play");
    }
    if (ui->ScrollArea_Huffman != nullptr){
        ui->ScrollArea_Huffman->viewport()->show();
    }
}

void HuffmanEncodingWindow::ClearAll(){
    if (ui->TableWidget_HuffmanFrequency != nullptr){
        int ColumnCount = ui->TableWidget_HuffmanFrequency->columnCount();
        for (int Col = 0; Col < ColumnCount; Col++){
            ui->TableWidget_HuffmanFrequency->removeColumn(0);
        }
    }
    Core.ClearAll();
    if (ui->Content_Huffman != nullptr){
        ui->Content_Huffman->ClearAll();
        ui->Content_Huffman->update();
    }
    if (ui->TextBrowser_OutputWords != nullptr){
        ui->TextBrowser_OutputWords->clear();
    }
}

void HuffmanEncodingWindow::GenerateFrequencyMap(){
    if (ui->LineEdit_OriginWords == nullptr || ui->TableWidget_HuffmanFrequency == nullptr){
        return;
    }
    std::unordered_map<char, int32_t> FrequencyMap = Core.GenerateFrequencyMap(ui->LineEdit_OriginWords->text().toStdString());
    for (auto Iter = FrequencyMap.begin(); Iter != FrequencyMap.end(); Iter++){
        AddFrequencyColumn(QString(Iter->first), Iter->second);
    }
}

void HuffmanEncodingWindow::AddFrequencyColumn(const QString& InWord, const int32_t& InFrequency){
    if (ui->TableWidget_HuffmanFrequency == nullptr){
        return;
    }
    QString StrWord(InWord);
    QString StrFrequency(QString::number(InFrequency));
    QTableWidgetItem* WordItem = new QTableWidgetItem(StrWord);
    QTableWidgetItem* FrequencyItem = new QTableWidgetItem(StrFrequency);
    int32_t Index = 0;
    while (Index < ui->TableWidget_HuffmanFrequency->colorCount()){
        QTableWidgetItem* TempFrequencyItem = ui->TableWidget_HuffmanFrequency->item(1, Index);
        if (TempFrequencyItem == nullptr){
            break;
        }
        int32_t TempFrequency = TempFrequencyItem->text().toInt();
        if (TempFrequency >= InFrequency){
            break;
        }
        Index++;
    }
    ui->TableWidget_HuffmanFrequency->insertColumn(Index);
    ui->TableWidget_HuffmanFrequency->setItem(0, Index, WordItem);
    ui->TableWidget_HuffmanFrequency->setItem(1, Index, FrequencyItem);
}

void HuffmanEncodingWindow::RemoveFrequencyColumn(const QString& RemoveWord){
    if (ui->TableWidget_HuffmanFrequency == nullptr){
        return;
    }
    int32_t RemoveIndex = -1;
    for (int32_t ColIndex = 0; ColIndex < ui->TableWidget_HuffmanFrequency->columnCount(); ColIndex++){
        QTableWidgetItem* Item = ui->TableWidget_HuffmanFrequency->item(0, ColIndex);
        if (Item != nullptr){
            // qDebug() << " Row: 0" << " Col: " << ColIndex << " Text: " << Item->data(Qt::ItemDataRole::EditRole).toString();
            if (Item->text() == RemoveWord){
                RemoveIndex = ColIndex;
                break;
            }
        }
    }
    if (RemoveIndex >= 0){
        ui->TableWidget_HuffmanFrequency->removeColumn(RemoveIndex);
    }
}

void HuffmanEncodingWindow::GeneratedEncodingResult(){

    if (ui->LineEdit_OriginWords != nullptr && ui->TextBrowser_OutputWords != nullptr){
        std::string EncodingResult = "";
        for (char Ch : ui->LineEdit_OriginWords->text().toStdString()){
            std::string StrCh = "";
            StrCh += Ch;
            std::string ChEncodingResult = Core.GetEncodingResult(StrCh);
            if (ui->Content_Huffman != nullptr){
                ui->Content_Huffman->AddEncodingResult(StrCh.c_str(), ChEncodingResult.c_str());
            }
            EncodingResult += ChEncodingResult;
        }
        ui->TextBrowser_OutputWords->setText(EncodingResult.c_str());
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

    std::vector<std::shared_ptr<HuffmanEncoding::Process>> StepForwardArray = Core.StepForward();
    if (StepForwardArray.empty()){
        return;
    }
    if (StepForwardArray[0]->Class == HuffmanEncoding::ProcessClass::ETreeNode){
        if (StepForwardArray.size() != 3){
            return;
        }
        QString LeafNode01;
        QString LeafNode02;
        QString RootNode;
        for (std::shared_ptr<HuffmanEncoding::Process> Process : StepForwardArray){
            std::shared_ptr<HuffmanEncoding::TreeNodeProcess> TreeNodeProcess = std::static_pointer_cast<HuffmanEncoding::TreeNodeProcess>(Process);
            if (TreeNodeProcess->Type == HuffmanEncoding::ProcessType::EAdd){
                if (!RootNode.isEmpty()){
                    return;
                }
                if (ui->TableWidget_HuffmanFrequency != nullptr){
                    AddFrequencyColumn(QString(TreeNodeProcess->Str.c_str()), TreeNodeProcess->Frequency);
                }
                RootNode = QString(TreeNodeProcess->Str.c_str());
            }
            else if (TreeNodeProcess->Type == HuffmanEncoding::ProcessType::ERemove){
                if (ui->TableWidget_HuffmanFrequency != nullptr){
                    RemoveFrequencyColumn(QString(TreeNodeProcess->Str.c_str()));
                    if (LeafNode01.isEmpty()){
                        LeafNode01 = QString(TreeNodeProcess->Str.c_str());
                    }
                    else if (LeafNode02.isEmpty()){
                        LeafNode02 = QString(TreeNodeProcess->Str.c_str());
                    }
                    else{
                        return;
                    }
                }
            }
        }
        if (ui->Content_Huffman != nullptr){
            ui->Content_Huffman->AddNodes(RootNode, LeafNode01, LeafNode02);
            ui->Content_Huffman->update();
        }
    }
    else if (StepForwardArray[0]->Class == HuffmanEncoding::ProcessClass::EBinary){
        for (std::shared_ptr<HuffmanEncoding::Process> Process : StepForwardArray){
            std::shared_ptr<HuffmanEncoding::BinaryProcess> BinaryProcess = std::static_pointer_cast<HuffmanEncoding::BinaryProcess>(Process);
            if (BinaryProcess->Type == HuffmanEncoding::ProcessType::EAdd){
                QString EdgeWord = QString(BinaryProcess->StrTarget.c_str());
                QString EdgeValue = QString(BinaryProcess->Value.c_str());
                if (ui->Content_Huffman != nullptr){
                    ui->Content_Huffman->AddEdgeWord(EdgeWord, EdgeValue);
                    ui->Content_Huffman->update();
                }
            }
        }
    }
    else{
        return;
    }
    if (Core.IsBinaryBuildCompleted()){
        GeneratedEncodingResult();
        QMessageBox::information(this, QString("Complete"), QString("Complete!"));
    }
}

void HuffmanEncodingWindow::StepBack(){

    std::vector<std::shared_ptr<HuffmanEncoding::Process>> StepBackArray = Core.StepBack();
    if (StepBackArray.empty()){
        return;
    }
    if (StepBackArray[0]->Class == HuffmanEncoding::ProcessClass::ETreeNode){
        for (std::shared_ptr<HuffmanEncoding::Process> StepBackProcess : StepBackArray){
            std::shared_ptr<HuffmanEncoding::TreeNodeProcess> TreeNodeProcess = std::static_pointer_cast<HuffmanEncoding::TreeNodeProcess>(StepBackProcess);
            std::vector<std::shared_ptr<HuffmanEncoding::TreeNodeProcess>> AddProcessArray;
            if (TreeNodeProcess->Type == HuffmanEncoding::ProcessType::ERemove){
                RemoveFrequencyColumn(TreeNodeProcess->Str.c_str());
                if (ui->Content_Huffman != nullptr){
                    ui->Content_Huffman->RemoveSphere(TreeNodeProcess->Str.c_str());
                    ui->Content_Huffman->update();
                }
            }
            else if (TreeNodeProcess->Type == HuffmanEncoding::ProcessType::EAdd){
                AddFrequencyColumn(TreeNodeProcess->Str.c_str(), TreeNodeProcess->Frequency);
                if (ui->Content_Huffman != nullptr){
                    ui->Content_Huffman->RemoveLine(TreeNodeProcess->Str.c_str());
                    if (TreeNodeProcess->Str.size() == 1){
                        ui->Content_Huffman->RemoveSphere(TreeNodeProcess->Str.c_str());
                    }
                    ui->Content_Huffman->update();
                }
            }
        }
    }
    else if (StepBackArray[0]->Class == HuffmanEncoding::ProcessClass::EBinary){
        for (std::shared_ptr<HuffmanEncoding::Process> StepBackProcess : StepBackArray){
            std::shared_ptr<HuffmanEncoding::BinaryProcess> BinaryProcess = std::static_pointer_cast<HuffmanEncoding::BinaryProcess>(StepBackProcess);
            if (BinaryProcess->Type == HuffmanEncoding::ProcessType::ERemove){
                if (ui->Content_Huffman != nullptr){
                    ui->Content_Huffman->RemoveEdgeWord(BinaryProcess->StrTarget.c_str());
                    ui->Content_Huffman->update();
                }
            }
        }
    }

}


