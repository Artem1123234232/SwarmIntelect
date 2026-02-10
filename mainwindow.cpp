#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedHeight(world.Size.y);
    setMinimumWidth(world.Size.x+200);

    ui->DrawSimulation->changeSize(world.Size.x, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
    ui->centralwidget->layout()->invalidate();

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
    //painter.fillRect(rect(), Qt::black);
    //painter.setBrush(Qt::black);
    world.draw(painter);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ClockSlider_valueChanged(int value)
{
    world.clock=value;
    timer->setInterval(value);

}


void MainWindow::on_BotCountSlider_valueChanged(int value)
{
    ui->BotCountLabel->setText(QString::number(value));
    world.NormalBotCount=value;
}


void MainWindow::on_ScoutCountSlider_valueChanged(int value)
{
    ui->ScoutCountLabel->setText(QString::number(value));
    world.ScoutBotCount=value;
}


void MainWindow::on_EliteCountSlider_valueChanged(int value)
{
    /*if (value > x) {
        // Блокируем дальнейшее движение
        ui->BotCountSlider->setValue(x);
        return; // Выходим, чтобы не выполнять логику ниже
    }*/
    ui->EliteCountLabel->setText(QString::number(value));
    world.EliteBotCount=value;
}

