#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setMouseTracking(true);
    imageWidget = ui->image_widget;

    // imageWidget->setStyleSheet(
    //     "QWidget {"
    //     "   border: 2px solid black;"
    //     "   border-radius: 5px;"
    //     "   background-color: transparent;" // Прозрачный фон
    //     "}"
    //     );
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_button_load_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Open Image", "",
                                                "Images (*.png *.jmg *.bmp)");
    if (!path.isEmpty()) {
        loadImage(path);
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1) {
    imageWidget->setDrawing(arg1 == Qt::Checked);
}

void MainWindow::on_button_plus_clicked() {
    if (imageWidget->getScale() >= 3.0) return;

    QPointF imageCenter = imageWidget->getImageCenter();

    QPointF widgetCenterBefore = imageWidget->imageToWidgetPos(imageCenter);

    imageWidget->setScale(imageWidget->getScale() + 0.25);

    // Преобразуем в координаты виджета ПОСЛЕ масштабирования
    QPointF widgetCenterAfter = imageWidget->imageToWidgetPos(imageCenter);

    imageWidget->setOffset(imageWidget->getOffset() +
                           (widgetCenterBefore - widgetCenterAfter));
}

void MainWindow::on_button_minus_clicked() {
    if (imageWidget->getScale() <= 0.25) return;

    QPointF imageCenter = imageWidget->getImageCenter();

    QPointF widgetCenterBefore = imageWidget->imageToWidgetPos(imageCenter);

    imageWidget->setScale(imageWidget->getScale() - 0.25);

    // Преобразуем в координаты виджета ПОСЛЕ масштабирования
    QPointF widgetCenterAfter = imageWidget->imageToWidgetPos(imageCenter);

    imageWidget->setOffset(imageWidget->getOffset() +
                           (widgetCenterBefore - widgetCenterAfter));
}

void MainWindow::loadImage(const QString &path) {
    imageWidget->clearImage();
    ui->checkBox->setChecked(false);
    ;
    originalImage.load(path);
    if (originalImage.isNull()) {
        qWarning() << "Failed to load image: " << path << "\n";
        return;
    }

    // Устанавливаем изображение в ImageWidget
    imageWidget->setImage(originalImage);
    imageWidget->setScale(1.0);
    imageWidget->setOffset(QPointF(0, 0));
}

// Начало рисования/перемещения
void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF widgetPos = imageWidget->mapFromParent(event->pos());

        if (imageWidget->isPointOnImage(widgetPos)) {
            QPointF imagePos = imageWidget->widgetToImagePos(widgetPos);

            // если включен чекбокс
            if (imageWidget->getDrawing()) {
                // Начало рисования прямоугольника
                startDrawing(imagePos);
            } else {
                updateCoordinates(imagePos);
                // Начало перемещения изображения
                isPanning = true;
                lastMousePos = event->pos();
            }
        } else {
            updateCoordinates(QPointF(-1, -1));
        }
    }
}

// обработка движения мыши
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QPointF widgetPos = imageWidget->mapFromParent(event->pos());

    if (isPanning) {
        // перемещение изображения
        QPointF delta = event->pos() - lastMousePos;
        // imageOffset += delta;
        imageWidget->setOffset(imageWidget->getOffset() + delta);
        lastMousePos = event->pos();
    } else if (imageWidget->getDrawing()) {
        // Рисование прямоугольника
        if (imageWidget->isPointOnImage(widgetPos)) {
            QPointF imagePos = imageWidget->widgetToImagePos(widgetPos);
            updateDrawing(imagePos);
        }
    }

    if (imageWidget->isPointOnImage(widgetPos)) {
        QPointF imagePos = imageWidget->widgetToImagePos(widgetPos);
        updateDrawing(imagePos);
    } else {
        updateCoordinates(QPointF(-1, -1));
    }
}

// Отпускание кнопки мыши
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (imageWidget->getDrawing()) {
            // завершение рисования
            finishDrawing();
        }
        isPanning = false;
    }
}

// Начало рисования прямоугольника
void MainWindow::startDrawing(const QPointF &pos) {
    imageWidget->startDrawing(pos);
}

// Обновление прямоугольника
void MainWindow::updateDrawing(const QPointF &pos) {
    imageWidget->updateDrawing(pos);
}

// Завершение рисования
void MainWindow::finishDrawing() {
    imageWidget->finishDrawing();
    ui->checkBox->setChecked(false);
}

void MainWindow::updateCoordinates(const QPointF &pos) {
    if (pos.x() < 0 || pos.y() < 0) {
        ui->label_x->setText("x: -");
        ui->label_y->setText("y: -");
    } else {
        ui->label_x->setText(
            "x: " + QString::number(static_cast<int>(pos.x()), 'f', 1));
        ui->label_y->setText(
            "y: " + QString::number(static_cast<int>(pos.y()), 'f', 1));
    }
}
