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
    void on_splitEdgeButton_clicked();
    void on_triangulateButton_clicked();
    void on_subdivideButton_clicked();
    void on_extrudeButton_clicked();
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
    void finishMeshEdit();
};


#endif // MAINWINDOW_H
