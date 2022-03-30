#ifndef HUFFMANENCODINGWINDOW_H
#define HUFFMANENCODINGWINDOW_H

#include <QWidget>
#include "HuffmanEncoding.h"

class QTimer;

namespace Ui {
class HuffmanEncodingWindow;
}

class HuffmanEncodingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HuffmanEncodingWindow(QWidget *parent = nullptr);
    ~HuffmanEncodingWindow();

signals:

    void OnBackButtonClicked();

private slots:

    void BackButtonClicked();

    void CloseSelf();

    void InputWordsCompleted();

    void AutoStepForward();
    void AutoPlayPauseButtonClicked();
    void AutoStopButtonClicked();
    void AutoStepForwardButtonClicked();
    void AutoStepBackButtonClicked();

private:

    void InitializeConnection();
    void InitializeUI();

    void ClearAll();

    void GenerateFrequencyMap();
    void AddFrequencyColumn(const QString& InWord, const int32_t& InFrequency);
    void RemoveFrequencyColumn(const QString& RemoveWord);
    void GeneratedEncodingResult();
    void StartAutoState(bool bInAutoPlay);
    void EndAutoState();
    void StartAutoPlay();
    void EndAutoPlay();
    void AutoPause();
    void StepForward();
    void StepBack();

    bool bAutoState = false;
    bool bAutoPlay = false;

    static int32_t AutoPlayIntervalTime;

    Ui::HuffmanEncodingWindow *ui;
    HuffmanEncoding Core;
    QTimer* AutoPlayerTimer = nullptr;
    QString InputLine;

};

#endif // HUFFMANENCODINGWINDOW_H
