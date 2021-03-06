#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MazeWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitializeConnection();

signals:

    void OnMazeButtonClick();
    void OnEightQueenButtonClick();
    void OnHuffmanEncodingButtonClick();

private:
    Ui::MainWindow *ui;

private slots:

    void MazeButtonClick();
    void EightQueenButtonClick();
    void HuffmanEncodingButtonClick();

    void OpenMazeUI();
    void OpenEightQueenUI();
    void OpenHuffmanEncodingUI();
    void ShowMainWindow();

};
#endif // MAINWINDOW_H
