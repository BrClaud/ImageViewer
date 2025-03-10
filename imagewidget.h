#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPixmap>

class ImageWidget : public QWidget
{
public:
    ImageWidget(QWidget *parent = nullptr);

    void setImage(const QPixmap& pixmap);
    void setScale(qreal scale);
    void setOffset(const QPointF& offset);
    QPointF widgetToImagePos(const QPointF& widgetPos) const;
    bool isPointOnImage(const QPointF& widgetPos) const;

    void startDrawing(const QPointF& pos);
    void updateDrawing(const QPointF& pos);
    void finishDrawing();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap image;
    qreal scale = 1.0;
    QPointF offset;

    bool isDrawing = false;
    QRectF currentRect;
    QList<QRectF> savedRects;
};

#endif // IMAGEWIDGET_H
