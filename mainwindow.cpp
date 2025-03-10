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
    QString path = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jmg *.bmp" );
    if(!path.isEmpty()){
        loadImage(path);
        updateDisplay();
    }

}

void MainWindow::on_checkBox_stateChanged(int arg1)
{

}

void MainWindow::on_button_plus_clicked()
{

}

void MainWindow::on_button_minus_clicked()
{

}

void MainWindow::loadImage(const QString& path){
    originalImage.load(path);
    if(originalImage.isNull()){
        qWarning()<<"Failed to load image: "<<path<<"\n";
    }

    imageOffset = QPointF(0,0);
    imageScale = 1.0;
    savedRects.clear();
}

// Преобразование координат виджета в координаты изображения
QPointF MainWindow::widgetToImagePos(const QPointF& widgetPos) const{
    return (widgetPos - imageOffset) / imageScale;
}




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
