#include "Stack.h"

Node::Node(Cell value) {
    this->data = value;
    this->next = nullptr;
}

Stack::Stack() {
    this->top = nullptr;
    this->length = 0;
}

void Stack::push(Cell value) {
    Node* newNode = new Node(value);
    newNode->next = top;
    top = newNode;
    this->length++;
}

Cell Stack::pop() {
    Node* temp = this->top;
    top = top->next;
    this->length--;

    Cell popped = temp->data;
    delete temp;

    return popped;
}

int Stack::getLength() {
    return this->length;
}