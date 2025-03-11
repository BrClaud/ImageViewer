#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QPixmap>
#include <QWidget>

class ImageWidget : public QWidget {
   public:
    ImageWidget(QWidget *parent = nullptr);

    void setImage(const QPixmap &pixmap);
    void setScale(qreal scale);
    void setOffset(const QPointF &offset);
    void setDrawing(bool drawing);
    bool getDrawing() { return isDrawing; }
    qreal getScale() { return scale; }
    QPointF getOffset();
    QPointF getImageCenter() const;
    QPointF widgetToImagePos(const QPointF &widgetPos) const;
    QPointF imageToWidgetPos(const QPointF &imagePos) const;
    bool isPointOnImage(const QPointF &widgetPos) const;

    void startDrawing(const QPointF &pos);
    void updateDrawing(const QPointF &pos);
    void finishDrawing();
    void clearImage();

   protected:
    void paintEvent(QPaintEvent *event) override;

   private:
    QPixmap image;
    qreal scale = 1.0;
    QPointF offset;  // Смещение изображения

    bool isDrawing = false;  // Флаг режима рисования
    QRectF currentRect;
    QList<QRectF> savedRects;
};

#endif  // IMAGEWIDGET_H
