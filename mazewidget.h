#ifndef MAZEWIDGET_H
#define MAZEWIDGET_H

#include <QWidget>
#include <QRandomGenerator>
#include <QPainter>
#include <QStack>
#include <QSet>
#include <QVector>
#include <QThread>
#include <QDebug>
#include <QQueue>
#include <queue>
#include <QtMath>

#include "priorityqueue.h"

struct CellDirection;

class MazeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MazeWidget(QWidget *parent = nullptr);
    ~MazeWidget();
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void createMaze();
    void searchMazeDFS();
    void searchMazeBFS();
    void searchMazeBestFirstSearch();
    int searchMode = 0;

    static int getGridSize();
    static void setGridSize(int gridSize);
    void setComplexity(int complexity);

public slots:
    void searchDFSSlot();
    void searchBFSSlot();
    void searchBestFirstSearchSlot();
    void resetSlot();

protected:
    void paintEvent(QPaintEvent* event) override;


private:
    int *sections;
    int numSections;
    double cell_scale;
    int sectionsSize;
    QThread* searchThread;
    static int gridSize;
    static int scale;
    static int sleep_time;

    void initSections();
    inline int getSection(int x, int y);
    inline unsigned int getPriority(const int x, const int y);
    void startThread();

signals:

};

#endif // MAZEWIDGET_H
