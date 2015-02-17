#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

// TODO(brendan): Figure out a way to declare these template functions here,
// define them in linkedList.cpp, and not get a whackload of linker errors

template<typename T> struct Node {
  T *item;
  Node<T> *next;
};
//struct Node;

#if 0
template<typename T> Node<T> *
addToList(T *newitem, Node<T> *list);

template<typename T> Node<T> *
*deleteFromList(T *toDeleteItem, Node<T> *list);

template<typename T>
void traverseList(void (*f)(T *item), Node<T> *list);

template<typename T>
void traverseList(void (*f)(T *item, float dt), float dt, Node<T> *list);
#endif

// TODO(brendan): pass the address of list?
// NOTE(brendan): add the token to the list
template<typename T> Node<T> *
addToList(T *newItem, Node<T> *list) {
  if(newItem != NULL) {
    Node<T> *resultNode = (Node<T> *)malloc(sizeof(Node<T>));
    resultNode->item = newItem;
    resultNode->next = list;
    return resultNode;
  }
  else {
    return list;
  }
}

// NOTE(brendan): delete the first one of this token; returns the list
template<typename T> Node<T> *
deleteFromList(T *toDeleteItem, Node<T> *list) {
  if(list != NULL) {
    Node<T> *current;
    Node<T> *previous;
    for(current = list, previous = NULL; 
        current != NULL && current->item != toDeleteItem; 
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

template<typename T>
void traverseList(void (*f)(T *item), Node<T> *list) {
  for(Node<T> *current = list;
      current != NULL;
      current = current->next) {
    (*f)(current->item);
  }
}

template<typename T>
void traverseList(void (*f)(T *item, float dt), float dt, Node<T> *list) {
  for(Node<T> *current = list;
      current != NULL;
      current = current->next) {
    (*f)(current->item, dt);
  }
}

template<typename T>
T *reduceList(bool (*f)(T *listItem, T *item), T *newest, Node<T> *list) {
	for(; list != NULL; list = list->next) {
		if ( (*f)(list->item, newest) == true ) return list->item;	
	}
	return NULL;
}

#endif
