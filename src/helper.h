#ifndef HELPER_H
#define HELPER_H

struct Point {
  int x;
  int y;
};

struct Rectangle {
  Point topLeft;
  Point bottomRight;
};

struct Circle {
  Point center;
  int radius;
};

// NOTE(brendan): INPUT: two integers x and y
// OUTPUT: the maximum of those two integers
inline int
maximum(int x, int y) 
{
  return (x > y) ? x : y;
}

#endif /* HELPER_H */
