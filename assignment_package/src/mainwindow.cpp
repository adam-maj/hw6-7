#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QFileDialog>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->vertsListWidget, &QListWidget::itemClicked, this, &MainWindow::selectVertex);
    connect(ui->halfEdgesListWidget, &QListWidget::itemClicked, this, &MainWindow::selectHalfEdge);
    connect(ui->facesListWidget, &QListWidget::itemClicked, this, &MainWindow::selectFace);
    connect(ui->mygl, &MyGL::selectionChanged, this, &MainWindow::updateSelection);
    connect(ui->vertPosXSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateVertexPosition);
    connect(ui->vertPosYSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateVertexPosition);
    connect(ui->vertPosZSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateVertexPosition);
    connect(ui->faceRedSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateFaceColor);
    connect(ui->faceGreenSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateFaceColor);
    connect(ui->faceBlueSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateFaceColor);
    fillLists();
    updateSelection();
    ui->mygl->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_loadOBJButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load OBJ", QString(), "OBJ Files (*.obj)");
    
    if(filename.isEmpty()) {
        return;
    }
    
    clearLists();
    bool loaded = ui->mygl->loadOBJ(filename);
    fillLists();
    
    updateSelection();
    
    if(!loaded) {
        QMessageBox::warning(this, "Load OBJ", "Could not read this OBJ file.");
    }
    
    ui->mygl->setFocus();
}

void MainWindow::fillLists()
{
    for(const auto &vertex : ui->mygl->mesh.vertices) {
        ui->vertsListWidget->addItem(vertex.get());
    }

    for(const auto &edge : ui->mygl->mesh.halfEdges) {
        ui->halfEdgesListWidget->addItem(edge.get());
    }

    for(const auto &face : ui->mygl->mesh.faces) {
        ui->facesListWidget->addItem(face.get());
    }
}

void MainWindow::clearLists()
{
    while(ui->vertsListWidget->count() > 0) {
        ui->vertsListWidget->takeItem(0);
    }

    while(ui->halfEdgesListWidget->count() > 0) {
        ui->halfEdgesListWidget->takeItem(0);
    }

    while(ui->facesListWidget->count() > 0) {
        ui->facesListWidget->takeItem(0);
    }
}

void MainWindow::selectVertex(QListWidgetItem *item)
{
    ui->mygl->selectVertex(static_cast<Vertex*>(item));
}

void MainWindow::selectHalfEdge(QListWidgetItem *item)
{
    ui->mygl->selectHalfEdge(static_cast<HalfEdge*>(item));
}

void MainWindow::selectFace(QListWidgetItem *item)
{
    ui->mygl->selectFace(static_cast<Face*>(item));
}

void MainWindow::updateSelection()
{
    Vertex *vertex = ui->mygl->selectedVertex;
    HalfEdge *edge = ui->mygl->selectedHalfEdge;
    Face *face = ui->mygl->selectedFace;
    ui->vertsListWidget->setCurrentItem(vertex);
    ui->halfEdgesListWidget->setCurrentItem(edge);
    ui->facesListWidget->setCurrentItem(face);
    
    if(vertex) {
        ui->vertsListWidget->scrollToItem(vertex);
    }
    
    if(edge) {
        ui->halfEdgesListWidget->scrollToItem(edge);
    }
    
    if(face) {
        ui->facesListWidget->scrollToItem(face);
    }

    QDoubleSpinBox *positions[] {ui->vertPosXSpinBox, ui->vertPosYSpinBox, ui->vertPosZSpinBox};
    QDoubleSpinBox *colors[] {ui->faceRedSpinBox, ui->faceGreenSpinBox, ui->faceBlueSpinBox};
    for(int i = 0; i < 3; ++i) {
        positions[i]->blockSignals(true);
        positions[i]->setEnabled(vertex != nullptr);
        positions[i]->setValue(vertex ? vertex->position[i] : 0);
        positions[i]->blockSignals(false);
        colors[i]->blockSignals(true);
        colors[i]->setEnabled(face != nullptr);
        colors[i]->setValue(face ? face->color[i] : 0);
        colors[i]->blockSignals(false);
    }
}

void MainWindow::updateVertexPosition()
{
    if(!ui->mygl->selectedVertex) {
        return;
    }
    glm::vec3 position = ui->mygl->selectedVertex->position;
    if(sender() == ui->vertPosXSpinBox) {
        position.x = ui->vertPosXSpinBox->value();
    } else if(sender() == ui->vertPosYSpinBox) {
        position.y = ui->vertPosYSpinBox->value();
    } else if(sender() == ui->vertPosZSpinBox) {
        position.z = ui->vertPosZSpinBox->value();
    }
    ui->mygl->setVertexPosition(position);
}

void MainWindow::updateFaceColor()
{
    if(!ui->mygl->selectedFace) {
        return;
    }
    glm::vec3 color = ui->mygl->selectedFace->color;
    if(sender() == ui->faceRedSpinBox) {
        color.r = ui->faceRedSpinBox->value();
    } else if(sender() == ui->faceGreenSpinBox) {
        color.g = ui->faceGreenSpinBox->value();
    } else if(sender() == ui->faceBlueSpinBox) {
        color.b = ui->faceBlueSpinBox->value();
    }
    ui->mygl->setFaceColor(color);
}