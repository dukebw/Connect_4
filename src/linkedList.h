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

#endif
