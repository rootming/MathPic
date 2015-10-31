#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    test = new Drawer();
    test->calcPost();
    test->drawThread();

}

MainWindow::~MainWindow()
{
    delete ui;
}
