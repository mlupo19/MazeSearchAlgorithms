#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QWidget* widget = new QWidget;
    maze = new MazeWidget(widget);
    QGridLayout* layout = new QGridLayout(widget);
    layout->addWidget(maze, 0, 0);
    setCentralWidget(widget);

    connect(ui->actionSearchDFS, SIGNAL(triggered()), maze, SLOT(searchDFSSlot()));
    connect(ui->actionSearchBFS, SIGNAL(triggered()), maze, SLOT(searchBFSSlot()));
    connect(ui->actionSearchBestFirstSearch, SIGNAL(triggered()), maze, SLOT(searchBestFirstSearchSlot()));
    connect(ui->actionReset, SIGNAL(triggered()), maze, SLOT(resetSlot()));
    connect(ui->actionComplexity, SIGNAL(triggered()), this, SLOT(changeComplexity()));
}

MainWindow::~MainWindow() {
    delete ui;
    delete maze;
}

void MainWindow::changeComplexity() {
    bool ok;
    int num = QInputDialog::getInt(this, tr("Maze complexity"), tr("Enter new maze complexity: "), QLineEdit::Normal, 16, 200, 16, &ok);

    if (ok)
        maze->setComplexity(num);
}
