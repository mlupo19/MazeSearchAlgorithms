#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    maze = new MazeWidget;

    ui->gridLayout->addWidget(maze, 0, 0);
    connect(ui->actionSearchDFS, SIGNAL(triggered()), maze, SLOT(searchDFSSlot()));
    connect(ui->actionSearchBFS, SIGNAL(triggered()), maze, SLOT(searchBFSSlot()));
    connect(ui->actionReset, SIGNAL(triggered()), maze, SLOT(resetSlot()));
}

MainWindow::~MainWindow() {
    delete ui;
}

