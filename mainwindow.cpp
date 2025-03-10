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
        updateDisplay();
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
    updateDisplay();
}

void MainWindow::on_button_minus_clicked()
{
    if(imageScale <= 0.25) return;
    imageScale -= 0.25;
    updateDisplay();
}

void MainWindow::loadImage(const QString& path){
    originalImage.load(path);
    if(originalImage.isNull()){
        qWarning()<<"Failed to load image: "<<path<<"\n";
    }

    imageOffset = QPointF(0,0);
    imageScale = 1.0;
    savedRects.clear();
    width_img = QPixmap(path).width();
    height_img = QPixmap(path).height();
}

// Преобразование координат виджета в координаты изображения
QPointF MainWindow::widgetToImagePos(const QPointF& widgetPos) const{
    return (widgetPos - imageOffset) / imageScale;
}



// Отрисовка всего содержимого
void MainWindow::paintEvent(QPaintEvent* event) {
    QMainWindow::paintEvent(event);
    QPainter painter(this);

    if (!originalImage.isNull()) {
        // Рисуем изображение с учетом масштаба и смещения
        painter.save();
        painter.translate(imageOffset);
        painter.scale(imageScale, imageScale);
        painter.drawPixmap(0, 0, originalImage);
        painter.restore();

        // Рисуем все сохраненные прямоугольники
        painter.setPen(QPen(Qt::red,2));
        for (const QRectF& rect : savedRects) {
            QRectF scaledRect = rect;
            scaledRect.moveTo(rect.topLeft() * imageScale + imageOffset);
            scaledRect.setSize(rect.size() * imageScale);
            painter.drawRect(scaledRect);
        }

        if(isDrawing){
            painter.setPen(QPen(Qt::blue,2));
            QRectF scaledRect = currentRect;
            scaledRect.moveTo(currentRect.topLeft() * imageScale + imageOffset);
            scaledRect.setSize(currentRect.size() * imageScale);
            painter.drawRect(scaledRect);
        }
    }
}

void MainWindow::updateDisplay(){
    update();
}
// Начало рисования/перемещения
void MainWindow::mousePressEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton){
        // если включен чекбокс
        if(isDrawing){
            // Начало рисования прямоугольника
            startDrawing(widgetToImagePos(event->pos()));
        }else{
            // Начало перемещения изображения
            isPanning = true;
            lastMousePos = event->pos();
        }
        // записываем координаты в лейблы
        QPointF currentPos = event->pos();
        QPointF imagePos = widgetToImagePos(currentPos);
        if(imagePos.x() > width_img || imagePos.x() < 0 || imagePos.y() > height_img || imagePos.y() < 0)
        {
            ui->label_x->setText("x: -");
            ui->label_y->setText("y: -");
        }else{
            ui->label_x->setText("x: " + QString::number(imagePos.x(), 'f', 1));
            ui->label_y->setText("y: " + QString::number(imagePos.y(), 'f', 1));
        }
    }
}

// обработка движения мыши
void MainWindow::mouseMoveEvent(QMouseEvent* event){
    QPointF currentPos = event->pos();

    if(isPanning){
        // перемещение изображения
        QPointF delta = currentPos - lastMousePos;
        imageOffset += delta;
        lastMousePos = currentPos;
        updateDisplay();
    }else if(isDrawing){
        updateDrawing(widgetToImagePos(currentPos));
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
    currentRect.setTopLeft(pos);
    currentRect.setSize(QSizeF(0,0));
}

// Обновление прямоугольника
void MainWindow::updateDrawing(const QPointF& pos){
    currentRect.setBottomRight(pos);
    updateDisplay();
}


// Завершение рисования
void MainWindow::finishDrawing(){
    savedRects.append(currentRect.normalized());
    isDrawing = false;
    ui->checkBox->setChecked(false);
    updateDisplay();
}

