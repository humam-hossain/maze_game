#ifndef STACK_H
#define STACK_H

#include "Cell.h"

class Node {
public:
    Cell data;
    Node* next;

    Node(Cell);
};

class Stack {
public:
    Stack();
    void push(Cell);
    Cell pop();
    int getLength();

private:
    Node* top;
    int length;
};

#endif // !STACK_H
