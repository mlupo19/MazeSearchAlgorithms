#include "priorityqueue.h"

PriorityQueue::PriorityQueue() {
    head = nullptr;
}

PriorityQueue::~PriorityQueue() {
    Node* node = head, *prev = nullptr;
    while (node != nullptr) {
        prev = node;
        node = node->next;
        delete prev;
    }
}

bool PriorityQueue::contains(int value) {
    if (head == nullptr)
        return false;
    
    Node* node = head;
    while (node != nullptr) {
        if (node->value == value)
            return true;
        node = node->next;
    }
    return false;
}

void PriorityQueue::enqueue(int value, unsigned int priority) {
    Node* node = head, *prev = nullptr;

    while (node != nullptr && node->priority < priority) {
        prev = node;
        node = node->next;
    }
    
    Node* newNode = new Node{value, priority, node};

    // If first element
    if (prev == nullptr) {
        head = newNode;
    } else {
        // Not first element
        prev->next = newNode;
    }
}

int PriorityQueue::dequeue() {
    if (head == nullptr) {
        return -1;
    }
    int out = head->value;
    Node* oldHead = head;
    head = head->next;
    delete oldHead;
    return out;
}

bool PriorityQueue::empty() {
    return head == nullptr;
}


