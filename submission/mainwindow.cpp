#include "mainwindow.h"
#include "qevent.h"
#include "ui_mainwindow.h"
#include "utility/painter.h"
#include "utility/darkeners.h"
#include <../mocks_library/prac/QTimer>


MainWindow::MainWindow(Game& game, Controller& controller, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , game_(game)
    , controller_{controller}
{
    ui->setupUi(this);
    controller_.SetRedrawCallback([this](){this->repaint();});

    prac::QTimer* gameTimer = new prac::QTimer(this);
    connect(gameTimer, &prac::QTimer::timeout, this, [this](){controller_.Tick();});
    gameTimer->start(40);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return QWidget::keyPressEvent(event);
    }
    switch (event->key()) {
    case Qt::Key_Right:
        controller_.OnMoveKey(Direction::kRight);
        break;
    case Qt::Key_Left:
        controller_.OnMoveKey(Direction::kLeft);
        break;
    case Qt::Key_Up:
        controller_.OnMoveKey(Direction::kUp);
        break;
    case Qt::Key_Down:
        controller_.OnMoveKey(Direction::kDown);
        break;
    default:
        QWidget::keyPressEvent(event); // call base class implementation
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event){
    switch (event->key()){
    case Qt::Key_Right:
        controller_.OnReleaseMoveKey(Direction::kRight);
        break;
    case Qt::Key_Left:
        controller_.OnReleaseMoveKey(Direction::kLeft);
        break;
    case Qt::Key_Up:
        controller_.OnReleaseMoveKey(Direction::kUp);
        break;
    case Qt::Key_Down:
        controller_.OnReleaseMoveKey(Direction::kDown);
        break;
    default:
        QWidget::keyReleaseEvent(event); // call base class implementation
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter qpainter(this);
    auto player_pos = game_.GetPlayer().GetExactPos();
    auto dir = game_.GetPlayer().GetDirection();
    int radius = 7;
    int distance = 4;
    FlashlightDarkener flashlight(player_pos, radius, dir, distance);

    //Инициализируем графический движок
    Painter  game_painter(qpainter);
    auto field_size = game_.GetField().GetRect();
    Size window_size{width(), height()};

    //Насройка графического движка
    game_painter.PreparePaint(player_pos, field_size, window_size);

    DrawContext context{
        .painter = game_painter,
        .darkener = flashlight
    };

    //Отрисовка
    game_.DrawFrame(context);
}
