#include "imagewidget.h"

#include <QPainter>
#include <QPainterPath>

ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent) {}

void ImageWidget::setImage(const QPixmap &pixmap) {
    if (pixmap.isNull()) return;

    image = pixmap;

    // Подгоняем изображение под размеры виджета
    QSize widgetSize = size();
    QSize imageSize = image.size();

    if (imageSize.width() > widgetSize.width() ||
        imageSize.height() > widgetSize.height()) {
        // Если изображение больше виджета, уменьшаем его
        image = image.scaled(widgetSize, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
    }

    update();
}

void ImageWidget::setScale(qreal scale) {
    this->scale = scale;
    update();
}

void ImageWidget::setOffset(const QPointF &offset) {
    this->offset = offset;
    update();
}

void ImageWidget::setDrawing(bool drawing) { isDrawing = drawing; }
QPointF ImageWidget::getOffset() {
    if (!offset.isNull())
        return offset;
    else
        return QPointF(-1, -1);
}

QPointF ImageWidget::getImageCenter() const {
    if (image.isNull()) return QPointF(0, 0);
    return QPointF(image.width() / 2.0, image.height() / 2.0);
}

QPointF ImageWidget::imageToWidgetPos(const QPointF &imagePos) const {
    QPointF widgetPos = (imagePos * scale + offset);
    return QPointF(static_cast<int>(widgetPos.x()),
                   static_cast<int>(widgetPos.y()));
}

QPointF ImageWidget::widgetToImagePos(const QPointF &widgetPos) const {
    QPointF imagePos = (widgetPos - offset) / scale;
    return QPointF(static_cast<int>(imagePos.x()),
                   static_cast<int>(imagePos.y()));
}

bool ImageWidget::isPointOnImage(const QPointF &widgetPos) const {
    if (image.isNull()) return false;

    QPointF imagePos = widgetToImagePos(widgetPos);

    return imagePos.x() >= 0 && imagePos.y() >= 0 &&
           imagePos.x() < image.width() && imagePos.y() < image.height();
}

void ImageWidget::startDrawing(const QPointF &pos) {
    isDrawing = true;
    currentRect.setTopLeft(pos);
    currentRect.setSize(QSizeF(0, 0));
    update();
}

void ImageWidget::updateDrawing(const QPointF &pos) {
    currentRect.setBottomRight(pos);
    update();
}

void ImageWidget::finishDrawing() {
    savedRects.append(currentRect.normalized());
    isDrawing = false;
    update();
}

void ImageWidget::paintEvent(QPaintEvent *event) {
    // QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Создаем путь для обрезки
    QPainterPath borderPath;
    borderPath.addRoundedRect(rect(), 10, 10);
    painter.setPen(QPen(Qt::black, 5));
    painter.drawPath(borderPath);

    const int padding = 2;  // Отступ от краев виджета (в пикселях)
    const int borderWidth = 2;  // Толщина обводки

    // Область для обрезки изображения (меньше виджета на padding + borderWidth)
    QRect contentRect =
        rect().adjusted(padding + borderWidth, padding + borderWidth,
                        -padding - borderWidth, -padding - borderWidth);

    // Обрезаем изображение
    QPainterPath clipPath;
    clipPath.addRoundedRect(contentRect, 5, 5);
    painter.setClipPath(clipPath);

    if (!image.isNull()) {
        // Отрисовка изображения
        painter.translate(offset);
        painter.scale(scale, scale);
        painter.drawPixmap(0, 0, image);

        // Отрисовка сохраненных прямоугольников
        painter.setPen(QPen(Qt::red, 2));
        for (const QRectF &rect : savedRects) {
            painter.drawRect(rect);
        }

        // Отрисовка текущего прямоугольника (если есть)
        if (isDrawing) {
            painter.setPen(QPen(Qt::blue, 2));
            painter.drawRect(currentRect);
        }
    }
}

void ImageWidget::clearImage() {
    savedRects.clear();
    offset = QPointF(0, 0);
    scale = 1.0;
    // currentRect = QRectF();
    isDrawing = false;
}
