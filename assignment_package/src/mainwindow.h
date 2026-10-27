#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();
    void on_loadOBJButton_clicked();
    void selectVertex(QListWidgetItem *item);
    void selectHalfEdge(QListWidgetItem *item);
    void selectFace(QListWidgetItem *item);
    void updateSelection();
    void updateVertexPosition();
    void updateFaceColor();

private:
    Ui::MainWindow *ui;
    void clearLists();
    void fillLists();
};


#endif // MAINWINDOW_H
