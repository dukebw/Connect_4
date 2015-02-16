#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "graphics.h"

struct Node {
  FallingToken *token;
  Node *next;
};
//struct Node;

Node *addToList(FallingToken *newToken, Node *list);
Node *deleteFromList(FallingToken *toDeleteToken, Node *list);
void traverseList(void (*f)(FallingToken *token), Node *list);
void traverseList(void (*f)(FallingToken *token, float dt), float dt, Node *list);

#endif
