// Copyright: Tom Krebs.
// Mail: <tomkre13@gmail.com>.

#include "./Object.h"
#include <ncurses.h>
#include <stdio.h>
#include <utility>
#include <vector>
#include <iostream>

// ____________________________________________________________________________
void Isle::new_isle(std::vector<int> set) {
  _coord = std::pair<size_t, size_t>(set.at(0), set.at(1));
  _value = set.at(2);
}

// ____________________________________________________________________________
size_t Isle::getX() { return _coord.first; }

// ____________________________________________________________________________
size_t Isle::getY() { return _coord.second; }

// ____________________________________________________________________________
int Isle::getValue() { return _value; }

// ____________________________________________________________________________
void Isle::changeValue(bool plus) {
  if (!plus) {
    _value = _value - 1;
  } else { _value = _value + 1; }
}

// ____________________________________________________________________________
bool Isle::isValid() {
  if (_value > 0) {
    return true;
  } else { return false; }
}

// ____________________________________________________________________________
void Bridge::new_bridge(std::vector<int> bridge) {
  _startC = std::pair<size_t, size_t>(bridge.at(0), bridge.at(1));
  _endC = std::pair<size_t, size_t>(bridge.at(2), bridge.at(3));
}

// ____________________________________________________________________________
std::pair<size_t, size_t> Bridge::getStart() { return _startC; }

// ____________________________________________________________________________
std::pair<size_t, size_t> Bridge::getEnd() { return _endC; }
