#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "opencvthread.h"
using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

signals:
    void over();

private slots:
    void on_pushButton_enter_clicked();
    void setImage(QImage imgSrc, QImage imgChanged, float fps);

private:
    Ui::MainWindow *ui;
    OpenCVThread *thred_showImg;
    int dev;
    QImage img;
    bool opened;

};
#endif // MAINWINDOW_H
