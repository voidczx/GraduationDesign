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

private:
    Ui::MazeWindow *ui;
};

#endif // MAZEWINDOW_H
