#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qi2c = new QI2C(0x01,0x70,this);

    qi2c->writeByte8(0x01,0xFF);
}

MainWindow::~MainWindow()
{

}

