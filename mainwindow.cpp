#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    imageWidget = ui->image_widget;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_button_load_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jmg *.bmp)" );
    if(!path.isEmpty()){
        loadImage(path);
    }

}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    isDrawing = (arg1 == Qt::Checked);
}

void MainWindow::on_button_plus_clicked()
{
    if(imageScale >= 3.0) return;
    imageScale += 0.25;

    // Применяем масштабирование относительно центра
    QPointF center = imageWidget->rect().center();
    QPointF imageCenter = imageWidget->widgetToImagePos(center);
    imageWidget->setScale(imageScale);

    QPointF newCenter = imageWidget->widgetToImagePos(center);
    imageOffset += (imageCenter - newCenter) * imageScale;
    imageWidget->setOffset(imageOffset);
}

void MainWindow::on_button_minus_clicked()
{
    if(imageScale <= 0.25) return;
    imageScale -= 0.25;

    // Применяем масштабирование относительно центра
    QPointF center = imageWidget->rect().center();
    QPointF imageCenter = imageWidget->widgetToImagePos(center);
    imageWidget->setScale(imageScale);

    // // Корректируем смещение, чтобы центр оставался на месте
    QPointF newCenter = imageWidget->widgetToImagePos(center);
    imageOffset += (imageCenter - newCenter) * imageScale;
    imageWidget->setOffset(imageOffset);
}

void MainWindow::loadImage(const QString& path){
    originalImage.load(path);
    if(originalImage.isNull()){
        qWarning()<<"Failed to load image: "<<path<<"\n";
    }

    imageWidget->setImage(originalImage);
    imageScale = 1.0;
    imageWidget->setScale(imageScale);
    imageWidget->setOffset(imageOffset);
}

// Начало рисования/перемещения
void MainWindow::mousePressEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton){
        QPointF widgetPos = imageWidget->mapFromParent(event->pos());

        if(imageWidget->isPointOnImage(widgetPos)){
            QPointF imagePos = imageWidget->widgetToImagePos(widgetPos);

            // если включен чекбокс
            if(isDrawing){
                // Начало рисования прямоугольника
                startDrawing(imagePos);
            }else{
                updateCoordinates(imagePos);
                // Начало перемещения изображения
                isPanning = true;
                lastMousePos = event->pos();
            }
        }else
        {
            updateCoordinates(QPointF(-1,-1));
        }
    }
}

// обработка движения мыши
void MainWindow::mouseMoveEvent(QMouseEvent* event){
    QPointF widgetPos = imageWidget->mapFromParent(event->pos());

    if(isPanning){
        // перемещение изображения
        QPointF delta = event->pos() - lastMousePos;
        imageOffset += delta;
        imageWidget->setOffset(imageOffset);
        lastMousePos = event->pos();
    }else if(isDrawing){
        // Рисование прямоугольника
        if(imageWidget->isPointOnImage(widgetPos)){
            QPointF imagePos =imageWidget->widgetToImagePos(widgetPos);
            updateDrawing(imagePos);
        }
    }

    if(imageWidget->isPointOnImage(widgetPos)){
        QPointF imagePos = imageWidget->widgetToImagePos(widgetPos);
        updateDrawing(imagePos);
    }else
    {
        updateCoordinates(QPointF(-1,-1));
    }
}

// Отпускание кнопки мыши
void MainWindow::mouseReleaseEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton){
        if(isDrawing){
            // завершение рисования
            finishDrawing();
        }
        isPanning = false;
    }
}

// Начало рисования прямоугольника
void MainWindow::startDrawing(const QPointF& pos){
    imageWidget->startDrawing(pos);
}

// Обновление прямоугольника
void MainWindow::updateDrawing(const QPointF& pos){
    imageWidget->updateDrawing(pos);
}


// Завершение рисования
void MainWindow::finishDrawing(){
    imageWidget->finishDrawing();
}

void MainWindow::updateCoordinates(const QPointF& pos){
    if(pos.x() < 0 || pos.y() < 0){
        ui->label_x->setText("x: -");
        ui->label_y->setText("y: -");
    }else{
        ui->label_x->setText("x: " + QString::number(static_cast<int>(pos.x()), 'f', 1));
        ui->label_y->setText("y: " + QString::number(static_cast<int>(pos.y()), 'f', 1));
    }
}

