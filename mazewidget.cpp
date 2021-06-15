#include "mazewidget.h"

struct CellDirection {
    int cell;
    int direction;
    int backDirection;
};

void searchMazeThread(MazeWidget* mw) {
    if (mw->searchMode == 0) {
        mw->searchMazeDFS();
    } else if (mw->searchMode == 1) {
        mw->searchMazeBFS();
    }
}

MazeWidget::MazeWidget(QWidget *parent) : QWidget(parent) {
    searchThread = nullptr;
    initSections(32);
    createMaze();

    scale = 20;
}

void MazeWidget::initSections(int numSections) {
    this->numSections = numSections;
    this->sectionsSize = numSections * numSections;
    this->sections = new int[sectionsSize];

    // Add gaps to entrance and exit of maze
    // sections[sectionsSize - 1] = 0b1101;
    // sections[0] = 0b1011;
}

int MazeWidget::getSection(int x, int y) {
    return this->sections[y * numSections + x];
}

void MazeWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    for (int i = 0; i < numSections; i++) {
        for (int j = 0; j < numSections; j++) {
            if (sections[j * numSections + i] & 0b010000) painter.fillRect(QRect(scale * i, scale * j, scale, scale), QBrush(Qt::blue));
            if (sections[j * numSections + i] & 0b100000) painter.fillRect(QRect(scale * i, scale * j, scale, scale), QBrush(Qt::green));
            if (sections[j * numSections + i] & 0b000001) painter.drawLine(scale * i, scale * (j + 1), scale * (i + 1), scale * (j + 1));
            if (sections[j * numSections + i] & 0b000010) painter.drawLine(scale * (i + 1), scale * j, scale * (i + 1), scale * (j + 1));
            if (sections[j * numSections + i] & 0b000100) painter.drawLine(scale * i, scale * j, scale * (i + 1), scale * j);
            if (sections[j * numSections + i] & 0b001000) painter.drawLine(scale * i, scale * j, scale * i, scale * (j + 1));
        }
    }
}

void MazeWidget::createMaze() {
    for (int i = 0; i < sectionsSize; i++) {
        this->sections[i] = 0b1111;
    }

    QStack<int> discovered;
    QSet<int> visited;

    int initialCell = QRandomGenerator::global()->bounded(numSections);
    discovered.push(initialCell);
    visited.insert(initialCell);

    while (!discovered.empty()) {
        int section = discovered.pop();

        QVector<CellDirection> unvisitedNeighbors;

        // Cell above
        if (section - numSections >= 0 && !visited.contains(section - numSections)) {
            CellDirection cd{section - numSections, 0b1011, 0b1110};
            unvisitedNeighbors.append(cd);
        }
        // Cell below
        if (section + numSections < sectionsSize && !visited.contains(section + numSections)) {
            CellDirection cd{section + numSections, 0b1110, 0b1011};
            unvisitedNeighbors.append(cd);
        }
        // Cell left
        if (section - 1 >= 0 && section % numSections != 0 && !visited.contains(section - 1)) {
            CellDirection cd{section - 1, 0b0111, 0b1101};
            unvisitedNeighbors.append(cd);
        }
        // Cell right
        if (section + 1 < sectionsSize && section % numSections != numSections - 1 && !visited.contains(section + 1)) {
            CellDirection cd{section + 1, 0b1101, 0b0111};
            unvisitedNeighbors.append(cd);
        }

        if (!unvisitedNeighbors.empty()) {
            discovered.push(section);
            CellDirection chosen = unvisitedNeighbors.at(QRandomGenerator::global()->bounded(unvisitedNeighbors.size()));

            // Remove wall between section and chosen
            sections[chosen.cell] &= chosen.backDirection;
            sections[section] &= chosen.direction;

            visited.insert(chosen.cell);
            discovered.push(chosen.cell);
        }
    }
    update();
}

void MazeWidget::searchMazeDFS() {
    QStack<int> discovered;
    QSet<int> visited;

    discovered.push(0);
    visited.insert(0);

    int chosen = 0, section = 0;

    while (!discovered.empty() && chosen != sectionsSize - 1 && !QThread::currentThread()->isInterruptionRequested()) {
        section = discovered.pop();

        QVector<int> unvisitedNeighbors;

        // Cell above
        if (section - numSections >= 0 && !(sections[section] & 0b0100) && !visited.contains(section - numSections)) {
            unvisitedNeighbors.append(section - numSections);
        }
        // Cell below
        if (section + numSections < sectionsSize && !(sections[section] & 0b0001) && !visited.contains(section + numSections)) {
            unvisitedNeighbors.append(section + numSections);
        }
        // Cell left
        if (section - 1 >= 0 && section % numSections != 0 && !(sections[section] & 0b1000) && !visited.contains(section - 1)) {
            unvisitedNeighbors.append(section - 1);
        }
        // Cell right
        if (section + 1 < sectionsSize && section % numSections != numSections - 1 && !(sections[section] & 0b0010) && !visited.contains(section + 1)) {
            unvisitedNeighbors.append(section + 1);
        }

        if (!unvisitedNeighbors.empty()) {
            QThread::msleep(20);

            sections[chosen] &= 0b011111;
            sections[chosen] |= 0b010000;

            discovered.push(section);
            chosen = unvisitedNeighbors.at(0);

            sections[section] |= 0b010000;
            sections[chosen] |= 0b100000;

            visited.insert(chosen);
            discovered.push(chosen);
            update();
        }
    }
    if (QThread::currentThread()->isInterruptionRequested()) {
        qDebug() << "Thread interrupted.";
        sections[section] &= 0b101111;
        sections[chosen] &= 0b011111;
    } else
        qDebug() << "Done.";
}


void MazeWidget::searchMazeBFS() {
    QQueue<int> discovered;
    QSet<int> visited;

    discovered.enqueue(0);
    visited.insert(0);

    int chosen = 0, section = 0;

    while (!discovered.empty() && chosen != sectionsSize - 1 && !QThread::currentThread()->isInterruptionRequested()) {
        section = discovered.dequeue();

        QVector<int> unvisitedNeighbors;

        // Cell above
        if (section - numSections >= 0 && !(sections[section] & 0b0100) && !visited.contains(section - numSections)) {
            unvisitedNeighbors.append(section - numSections);
        }
        // Cell below
        if (section + numSections < sectionsSize && !(sections[section] & 0b0001) && !visited.contains(section + numSections)) {
            unvisitedNeighbors.append(section + numSections);
        }
        // Cell left
        if (section - 1 >= 0 && section % numSections != 0 && !(sections[section] & 0b1000) && !visited.contains(section - 1)) {
            unvisitedNeighbors.append(section - 1);
        }
        // Cell right
        if (section + 1 < sectionsSize && section % numSections != numSections - 1 && !(sections[section] & 0b0010) && !visited.contains(section + 1)) {
            unvisitedNeighbors.append(section + 1);
        }

        if (!unvisitedNeighbors.empty()) {
            QThread::msleep(20);

            sections[chosen] &= 0b011111;
            sections[chosen] |= 0b010000;

            discovered.enqueue(section);
            chosen = unvisitedNeighbors.at(0);

            sections[section] |= 0b010000;
            sections[chosen] |= 0b100000;

            visited.insert(chosen);
            discovered.enqueue(chosen);
            update();
        }
    }
    if (QThread::currentThread()->isInterruptionRequested()) {
        qDebug() << "Thread interrupted.";
        sections[section] &= 0b101111;
        sections[chosen] &= 0b011111;
    }
}

QSize MazeWidget::sizeHint() const {
    return QSize(scale * numSections * 1.2, scale * numSections * 1.2 );
}

void MazeWidget::searchDFSSlot() {
    searchMode = 0;
    searchThread = QThread::create(searchMazeThread, this);
    searchThread->start();
}

void MazeWidget::searchBFSSlot() {
    searchMode = 1;
    searchThread = QThread::create(searchMazeThread, this);
    searchThread->start();
}

void MazeWidget::resetSlot() {
    if (searchThread != nullptr && searchThread->isRunning()) {
        searchThread->requestInterruption();
    }
    createMaze();
}
