#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QPointF>
#include <QRectF>

#include "imagewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ImageWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_button_load_clicked();
  void on_checkBox_stateChanged(int arg1);
  void on_button_plus_clicked();
  void on_button_minus_clicked();

 protected:
  // обработчики
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

 private:
  void loadImage(const QString &path);
  void updateCoordinates(const QPointF &pos);
  void startDrawing(const QPointF &pos);
  void updateDrawing(const QPointF &pos);
  void finishDrawing();

  Ui::MainWindow *ui;
  QPixmap originalImage;  // Исходное изображение
  // QPointF imageOffset;      // Смещение изображения
  qreal imageScale = 1.0;  // Текущий масштаб (1.0 = 100%)
  bool isPanning = false;  // Флаг перемещения изображения
  // bool isDrawing = false;   // Флаг режима рисования
  QPointF lastMousePos;  // Последняя позиция мыши
  // QRectF currentRect;       // Текущий прямоугольник

  ImageWidget *imageWidget;
};
#endif  // MAINWINDOW_H
