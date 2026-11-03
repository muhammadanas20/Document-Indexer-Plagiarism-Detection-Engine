#include <iostream>
using namespace std;

struct Node {
  int data;
  Node* next;
};

Node* insertAtStart(Node* head, int value) {
  Node* newNode = new Node;
  newNode->data = value;
  newNode->next = head;
  return newNode;
}

void printList(Node* head) {
  Node* current = head;
  while (current != NULL) {
    cout << current->data << " -> ";
    current = current->next;
  }
  cout << "NULL" << endl;
}

int main() {
  Node* head = NULL;
  head = insertAtStart(head, 10);
  head = insertAtStart(head, 20);
  head = insertAtStart(head, 30);
  printList(head);
  return 0;
}
