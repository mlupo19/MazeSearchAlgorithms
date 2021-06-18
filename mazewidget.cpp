#include "mazewidget.h"

int MazeWidget::gridSize = 64;
int MazeWidget::scale = 640;
int MazeWidget::sleep_time = 20;

struct CellDirection {
    // cell index
    int cell;

    // wall side
    int direction;

    // opposite wall side
    int backDirection;
};

// Runs the selected search algorithm
void searchMazeThread(MazeWidget* mw) {
    if (mw->searchMode == 0) {
        mw->searchMazeDFS();
    } else if (mw->searchMode == 1) {
        mw->searchMazeBFS();
    } else if (mw->searchMode == 2) {
        mw->searchMazeBestFirstSearch();
    }
}

inline unsigned int MazeWidget::getPriority(const int x, const int y) {
    return 100 * (qFabs(x - numSections - 1) + qFabs(x - numSections - 1));
}

void MazeWidget::startThread() {
    if (searchThread != nullptr) {
        searchThread->requestInterruption();
        searchThread->wait(1000);
        delete searchThread;
    }
    searchThread = QThread::create(searchMazeThread, this);
    searchThread->start();
}

MazeWidget::MazeWidget(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    searchThread = nullptr;
    initSections();
    createMaze();
}

MazeWidget::~MazeWidget() {
    delete [] sections;
}

// Initializes the array to hold the cells
void MazeWidget::initSections() {
    cell_scale = 1.0 * scale / gridSize;
    this->numSections = gridSize;
    this->sectionsSize = numSections * numSections;
    this->sections = new int[sectionsSize];

    // Add gaps to entrance and exit of maze
    // sections[sectionsSize - 1] = 0b1101;
    // sections[0] = 0b1011;
}

inline int MazeWidget::getSection(int x, int y) {
    return this->sections[y * numSections + x];
}

void MazeWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    int widgetSize = cell_scale * numSections;

    for (int i = 0; i < numSections; i++) {
        for (int j = 0; j < numSections; j++) {
            if (sections[j * numSections + i] & 0b010000) painter.fillRect(QRect(cell_scale * i, cell_scale * j, cell_scale, cell_scale), QBrush(Qt::blue));
            if (sections[j * numSections + i] & 0b100000) painter.fillRect(QRect(cell_scale * i, cell_scale * j, cell_scale, cell_scale), QBrush(Qt::green));
            if (sections[j * numSections + i] & 0b000001) painter.drawLine(cell_scale * i, cell_scale * (j + 1), cell_scale * (i + 1), cell_scale * (j + 1));
            if (sections[j * numSections + i] & 0b000010) painter.drawLine(cell_scale * (i + 1), cell_scale * j, cell_scale * (i + 1), cell_scale * (j + 1));
            if (sections[j * numSections + i] & 0b000100) painter.drawLine(cell_scale * i, cell_scale * j, cell_scale * (i + 1), cell_scale * j);
            if (sections[j * numSections + i] & 0b001000) painter.drawLine(cell_scale * i, cell_scale * j, cell_scale * i, cell_scale * (j + 1));
        }
    }
}

void MazeWidget::createMaze() {
    for (int i = 0; i < sectionsSize; i++) {
        this->sections[i] = 0b1111;
    }

    QStack<int> discovered;
    QSet<int> visited;

    // Pick random cell to start from
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
            QThread::msleep(sleep_time);

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
    }
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
            QThread::msleep(sleep_time);

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

void MazeWidget::searchMazeBestFirstSearch() {
    PriorityQueue discovered;
    QSet<int> visited;

    discovered.enqueue(0, getPriority(0, 0));
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
            QThread::msleep(sleep_time);

            sections[chosen] &= 0b011111;
            sections[chosen] |= 0b010000;

            discovered.enqueue(section, getPriority(section % numSections, section / numSections));
            chosen = unvisitedNeighbors.at(0);

            sections[section] |= 0b010000;
            sections[chosen] |= 0b100000;

            visited.insert(chosen);
            discovered.enqueue(chosen, getPriority(chosen % numSections, chosen / numSections));
            update();
        }
    }
    if (QThread::currentThread()->isInterruptionRequested()) {
        qDebug() << "Thread interrupted.";
        sections[section] &= 0b101111;
        sections[chosen] &= 0b011111;
    }
}

int MazeWidget::getGridSize() {
    return gridSize;
}

void MazeWidget::setGridSize(int newGridSize) {
    gridSize = newGridSize;
}

void MazeWidget::setComplexity(int complexity) {
    if (getGridSize() != complexity) {
        setGridSize(complexity);
        if (searchThread != nullptr) {
            searchThread->requestInterruption();
            searchThread->wait(1000);
            delete searchThread;
            searchThread = nullptr;
        }

        delete [] sections;

        initSections();
        createMaze();
    }
}

QSize MazeWidget::sizeHint() const {
    return QSize(qCeil(1.0 * cell_scale * numSections) + 1, qCeil(1.0 * cell_scale * numSections) + 1);
}

QSize MazeWidget::minimumSizeHint() const {
    return QSize(qCeil(1.0 * cell_scale * numSections) + 1, qCeil(1.0 * cell_scale * numSections) + 1);
}

void MazeWidget::searchDFSSlot() {
    searchMode = 0;
    startThread();
}

void MazeWidget::searchBFSSlot() {
    searchMode = 1;
    startThread();
}

void MazeWidget::searchBestFirstSearchSlot() {
    searchMode = 2;
    startThread();
}

void MazeWidget::resetSlot() {
    if (searchThread != nullptr) {
        searchThread->requestInterruption();
        searchThread->wait(1000);
        delete searchThread;
        searchThread = nullptr;
    }
    createMaze();
}
