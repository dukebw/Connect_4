/********************************************
 * A Connect Four Game                      *
 * Designed by struct by_lightning{};       *
 * Group 6                                  *
 * Kuir Aguer, Brendan Duke, Jean Ferreira, *
 * Zachariah Levine and Pranesh Satish      *
 ********************************************/

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

template<typename T>
class List {
  T *item;
  List<T> *next;

  // NOTE(brendan): private constructor so that clients can't make a LIst
  // object except by using addToList(item, null);
  List() {}

public:
  // NOTE(Zach): empty/destroy the entire list
	static void emptyList(List<T> **list);

  // NOTE(brendan): add item T to the list
  static List<T> *
  addToList(T *newitem, List<T> *list);

  // NOTE(brendan): delete the first occurrence of item T; returns the list
  static List<T> *
  deleteFromList(T *toDeleteItem, List<T> *list);

  // NOTE(brendan): iterate over list, executing function f() on each node
  static void 
  traverseList(void (*f)(T *item), List<T> *list);

  // NOTE(brendan): iterate over list, executing function f() on each node
  static void 
  traverseList(void (*f)(T *item, float dt), float dt, List<T> *list);

  // NOTE(brendan): iterate over list, returning the item of the first node
  // that satisfies function f
  static T *
  reduceList(bool (*f)(T *listItem, T *item), T *newest, List<T> *list);
};

// NOTE(brendan): add item T to the list
template<typename T> List<T> *
List<T>::addToList(T *newItem, List<T> *list) {
  if(newItem != NULL) {
    List<T> *resultList = (List<T> *)malloc(sizeof(List<T>));
    resultList->item = newItem;
    resultList->next = list;
    return resultList;
  }
  else {
    return list;
  }
}

template<typename T> 
void List<T>::emptyList(List<T> **list) {
	List<T> *current = *list;
	List<T> *previous = NULL;
	while (current != NULL) {
		previous = current;
		current = current->next;
		free(previous);
	}
	*list = NULL;
}

// NOTE(brendan): delete the first occurence of item T; returns the list
template<typename T> List<T> *
List<T>::deleteFromList(T *toDeleteItem, List<T> *list) {
  if(list != NULL) {
    List<T> *current;
    List<T> *previous;
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

// NOTE(brendan): iterate over list, executing function f() on each node
template<typename T> void 
List<T>::traverseList(void (*f)(T *item), List<T> *list) {
  for(List<T> *current = list;
      current != NULL;
      current = current->next) {
    (*f)(current->item);
  }
}

// NOTE(brendan): iterate over list, executing function f() on each node
template<typename T> void 
List<T>::traverseList(void (*f)(T *item, float dt), float dt, List<T> *list) {
  for(List<T> *current = list;
      current != NULL;
      current = current->next) {
    (*f)(current->item, dt);
  }
}

// NOTE(brendan): iterate over list, returning the item of the first node
// that satisfies function f
template<typename T> T *
List<T>::reduceList(bool (*f)(T *listItem, T *item), T *newest, List<T> *list) {
	for(; list != NULL; list = list->next) {
		if((*f)(list->item, newest) == true ) {
      return list->item;	
    }
	}
	return NULL;
}

#endif
