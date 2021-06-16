#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

struct Node {
    int value;
    unsigned int priority;
    Node* next;
};

class PriorityQueue
{
public:
    PriorityQueue();
    ~PriorityQueue();
    bool contains(int value);
    void enqueue(int value, unsigned int priority);
    int dequeue();
    bool empty();

private:
    Node* head;
};

#endif // PRIORITYQUEUE_H
