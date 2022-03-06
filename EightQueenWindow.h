#ifndef EIGHTQUENEWINDOW_H
#define EIGHTQUENEWINDOW_H

#include <QWidget>

namespace Ui {
class EightQueenWindow;
}

class EightQueenWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EightQueenWindow(QWidget *parent = nullptr);
    ~EightQueenWindow();

signals:

    void OnBackButtonClicked();

private slots:

    void BackButtonClicked();

    void CloseSelf();

private:
    Ui::EightQueenWindow *ui;
};

#endif // EIGHTQUENEWINDOW_H
