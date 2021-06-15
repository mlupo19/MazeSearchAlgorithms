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

struct CellDirection;

class MazeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MazeWidget(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    void createMaze();
    void searchMazeDFS();
    void searchMazeBFS();
    int searchMode = 0;

public slots:
    void searchDFSSlot();
    void searchBFSSlot();
    void resetSlot();

protected:
    void paintEvent(QPaintEvent* event) override;


private:
    int *sections;
    int numSections;
    int scale;
    int sectionsSize;
    QThread* searchThread;

    void initSections(int sections);
    int getSection(int x, int y);

signals:

};

#endif // MAZEWIDGET_H
