#include "imagewidget.h"
#include <QPainter>


ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent) {}

void ImageWidget::setImage(const QPixmap& pixmap){
    image = pixmap;
    update();
}

void ImageWidget::setScale(qreal scale){
    this->scale = scale;
    update();
}

void ImageWidget::setOffset(const QPointF& offset){
    this->offset = offset;
    update();
}

QPointF ImageWidget::widgetToImagePos(const QPointF& widgetPos) const {
    QPointF imagePos = (widgetPos - offset) / scale;
    return QPointF(static_cast<int>(imagePos.x()), static_cast<int>(imagePos.y()));
}

bool ImageWidget::isPointOnImage(const QPointF& widgetPos) const{
    if(image.isNull()) return false;

    QPointF imagePos = widgetToImagePos(widgetPos);

    return imagePos.x()>= 0 && imagePos.y() >=0 &&
           imagePos.x() < image.width() && imagePos.y() < image.height();
}

void ImageWidget::startDrawing(const QPointF& pos) {
    isDrawing = true;
    currentRect.setTopLeft(pos);
    currentRect.setSize(QSizeF(0, 0));
    update();
}

void ImageWidget::updateDrawing(const QPointF& pos) {
    currentRect.setBottomRight(pos);
    update();
}

void ImageWidget::finishDrawing() {
    savedRects.append(currentRect.normalized());
    isDrawing = false;
    update();
}

void ImageWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    if (!image.isNull()) {
        QPainter painter(this);

        // Отрисовка изображения
        painter.translate(offset);
        painter.scale(scale, scale);
        painter.drawPixmap(0, 0, image);

        // Отрисовка сохраненных прямоугольников
        painter.setPen(QPen(Qt::red, 2));
        for (const QRectF& rect : savedRects) {
            painter.drawRect(rect);
        }

        // Отрисовка текущего прямоугольника (если есть)
        if (isDrawing) {
            painter.setPen(QPen(Qt::blue, 2));
            painter.drawRect(currentRect);
        }
    }
}


