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

#define MAX(x,y) ((x) < (y) ? (y) : (x))

#endif /* HELPER_H */
