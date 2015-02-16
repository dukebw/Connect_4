#include "linkedList.h"

// TODO(brendan): Move these to a data structures module
#if 0
struct Node {
  FallingToken *token;
  Node *next;
};
#endif

// TODO(brendan): pass the address of list?
// NOTE(brendan): add the token to the list
Node *addToList(FallingToken *newToken, Node *list) {
  if(newToken != NULL) {
    Node *resultNode = (Node *)malloc(sizeof(Node));
    resultNode->token = newToken;
    resultNode->next = list;
    return resultNode;
  }
  else {
    return list;
  }
}

// NOTE(brendan): delete the first one of this token; returns the list
Node *deleteFromList(FallingToken *toDeleteToken, Node *list) {
  if(list != NULL) {
    Node *current;
    Node *previous;
    for(current = list, previous = NULL; 
        current != NULL && current->token != toDeleteToken; 
        previous = current, current = current->next);
    if(current != NULL) {
      if(previous != NULL) {
        previous->next = current->next;
      }
      else {
        list = list->next;
      }
      free(current);
    }
    return list;
  }
  else {
    return NULL;
  }
}

void traverseList(void (*f)(FallingToken *token), Node *list) {
  for(Node *current = list;
      current != NULL;
      current = current->next) {
    (*f)(current->token);
  }
}
