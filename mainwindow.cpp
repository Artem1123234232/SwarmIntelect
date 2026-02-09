#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);

    // 2. Соединяем сигнал таймера (timeout) с нашим методом (nextStep)
    connect(timer, &QTimer::timeout, this, &MainWindow::nextStep);

    // 3. Запускаем! 16 мс — это примерно 60 кадров в секунду
    timer->start(world.clock);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    // Проверяем конкретные клавиши
    if (event->key() == Qt::Key_R) {
        world.reset();
        update();
    }
}

void MainWindow::nextStep() {
    if(world.tick(gen)){update();}
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);
    painter.setBrush(Qt::black);
    world.draw(painter);
}


MainWindow::~MainWindow()
{
    delete ui;
}

