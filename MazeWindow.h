#ifndef MAZEWINDOW_H
#define MAZEWINDOW_H

#include <QWidget>

namespace Ui {
class MazeWindow;
}

class MazeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MazeWindow(QWidget *parent = nullptr);
    ~MazeWindow();

signals:

    void OnBackButtonClick();

private slots:

    void CloseSelfWindow();
    void BackButtonClick();

private:
    Ui::MazeWindow *ui;
};

#endif // MAZEWINDOW_H
