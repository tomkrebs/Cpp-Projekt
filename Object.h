// Copyright: Tom Krebs.
// Mail: <tomkre13@gmail.com>.

#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdio.h>
#include <utility>
#include <vector>

class Object {
 public:
  virtual ~Object() {}

 protected:
  std::pair<size_t, size_t> _coord;
};

class Isle : public Object {
 public:
  virtual ~Isle() {}

  // Adds a new  isle to the class.
  void new_isle(std::vector<int> set);

  // Returns the X coordinate.
  size_t getX();

  // Returns the Y coordinate.
  size_t getY();

  // Returns the value of the isle.
  int getValue();

  // Counts the value up or down.
  // plus = +; !plus = -.
  void changeValue(bool plus);

  // Checks whether value is over zero.
  bool isValid();

 protected:
  int _value;
};

class Bridge : public Object {
 public:
  virtual ~Bridge() {}

  // Adds a new bridge to the class.
  void new_bridge(std::vector<int> bridge);

  // Returns the start coordinates of the bridge.
  std::pair<size_t, size_t> getStart();

  // Returns the end coordinates of the bridge.
  std::pair<size_t, size_t> getEnd();
 protected:
  // Coordinates of the start isle.
  std::pair<size_t, size_t> _startC;

  // Coordinates of the end isle.
  std::pair<size_t, size_t> _endC;
};
#endif  // OBJECT_H_
