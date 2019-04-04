// Copyright: Tom Krebs 2018
// Mail: <tomkre13@gmail.com>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <map>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "./Hashi.h"
#include "./Object.h"

// ____________________________________________________________________________
Hashi::Hashi() {
  _undos = 5;
  _inputFileName = "";
}

// ____________________________________________________________________________
Hashi::~Hashi() {
  endwin();
}

// ____________________________________________________________________________
void Hashi::printUsageAndExit() const {
  fprintf(stderr, "Usage: ./HashiMain (--undo (number)) <inputfile>\n");
  fprintf(stderr, "Available options:\n");
  fprintf(stderr, "-u <integer> : Set the amount of available undos.\n");
  fprintf(stderr, "(default: 5)\n");
  endwin();
  exit(1);
}

// ____________________________________________________________________________
void Hashi::parseCommandLineArguments(int argc, char** argv) {
  struct option options[] = {
    {"undo", 1, NULL, 'u'},
    {NULL, 0, NULL, 0}
  };
  optind = 1;

  // Default values.
  _undos = 5;
  _inputFileName = "";

  while (true) {
    // convert all elements in the command line without the filename.
    char c = getopt_long(argc, argv, "u", options, NULL);
    if (c == -1) { break; }
    switch (c) {
      case 'u':
        _undos = atoi(optarg);
        break;
      default:
        printUsageAndExit();
    }
  }

  // input file is required argument.
  if (optind + 1 != argc) {
    printUsageAndExit();
  }
  // read in the filename.
  _inputFileName = argv[optind];
}

// ____________________________________________________________________________
void Hashi::initializeGame() {
  // Initialising all important window settings.
  initscr();
  cbreak();
  noecho();
  curs_set(false);
  nodelay(stdscr, true);
  keypad(stdscr, true);
  mousemask(ALL_MOUSE_EVENTS, NULL);
  start_color();
  clear();

  // Green if isle has value 0.
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  // White if value is > 0.
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  // Red for value < 0.
  init_pair(3, COLOR_RED, COLOR_BLACK);

  Isle* isle = nullptr;
  // This vector contains all important information for sub-class Isle.
  std::vector<int> tmp_isle;
  tmp_isle.resize(3);

  // Open file and check whether it worked.
  std::ifstream file(_inputFileName.c_str());
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << _inputFileName << std::endl;
    endwin();
    exit(1);
  }

  int cnt = -1;
  std::string line;

  if (_inputFileName.substr(_inputFileName.find_last_of('.')) == ".plain") {
    // If we got a *.plain file.
    while (true) {
      cnt++;
      std::getline(file, line);
      // Don't care about comments.
      if (line[0] == '#') { continue; }
      // Stop as we reach the end of the file.
      if (file.eof()) { break; }
      for (size_t i = 1; i < line.length()+1; i++) {
        // Look at every sign in the file and if it isn't a whitespace we can
        // create a new isle.
        if (line[i-1] != ' ') {
          tmp_isle[0] = 5 * i;
          tmp_isle[1] = 5 * cnt;
          tmp_isle[2] = stoi(line.substr(i-1, 1));
          isle = new Isle();
          isle->new_isle(tmp_isle);
          _isles.push_back(isle);
          isle = nullptr;
        }
      }
    }
  } else if (_inputFileName.substr(_inputFileName.find_last_of('.')) == ".xy") {
    // Here the similiar procedure for a *.xy file.
    while (true) {
      std::getline(file, line);
      if (line[0] == '#') { continue; }
      // Comments are useless.
      if (file.eof()) { break; }
      // Stop as if we reach the end of file.

      // Every line is: x, y, value.
      size_t pos1 = line.find(",");
      size_t pos2 = line.find(",", pos1+1);

      tmp_isle[0] = 5 * (stoi(line.substr(0, pos1)) + 1);
      // x coordinate
      tmp_isle[1] = 5 * (stoi(line.substr(pos1 + 1, pos2 - pos1 - 1)) + 1);
      // y coordinate
      tmp_isle[2] = stoi(line.substr(pos2 + 1));
      // value

      isle = new Isle();
      isle->new_isle(tmp_isle);
      _isles.push_back(isle);
      isle = nullptr;
    }
  }
}

// ____________________________________________________________________________
void Hashi::printAround(size_t y, size_t x) {
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      mvprintw(y+j, x+i, " ");
    }
  }
}

// ____________________________________________________________________________
void Hashi::play() {
  bool vic_flag = false;
  // flag to get the 'Victory!!!' blinking.
  while (true) {
    int key = getch();
    // pressed key.
    processUserInput(key);
    // convert the information, key is giving.
    showState();
    // show actual state.
    if (victory()) {
      // if we finished we print a blinking 'victory!!!' out.
      vic_flag = !vic_flag;
      if (vic_flag) {
        color_set(1, 0);
      } else {
        color_set(2, 0);
      }
      mvprintw(0, 0, "Victory!!!");
    }
    refresh();
  }
}

// ____________________________________________________________________________
void Hashi::processUserInput(int key) {
  MEVENT event;
  switch (key) {
    case 27:
      // End programm with Escape.
      endwin();
      exit(1);
      break;
    case 'u':
      // undo a bridge when u is pressed.
      undo();
      break;
    case 's':
      // If you press s you can give the programm a solution file and it shows
      // the correct solution.
      endwin();
      // first close the ncurses window.
      std::cout << "Enter file: ";
      // Help the user, what to do next.
      std::cin >> _inputSolutionFileName;
      // Read the text and save it as solution file.
      _isles.clear();
      _bridges.clear();
      // before we start the window again and initialize all isles again,
      // we need to clear the old isles and bridges, so we don't have it twice.
      initializeGame();
      solve(_inputSolutionFileName);
      break;
    case KEY_MOUSE:
      // convert the mouse click.
      if (getmouse(&event) == OK) {
        if (event.bstate & BUTTON1_CLICKED) {
          _startIsleX = _lastClickedX;
          _startIsleY = _lastClickedY;
          _lastClickedX = event.x;
          _lastClickedY = event.y;
          newBridge(_startIsleX, _startIsleY, _lastClickedX, _lastClickedY);
          attron(A_REVERSE);
        }
      }
      break;
  }
}

// ____________________________________________________________________________
void Hashi::newBridge(int startX, int startY, int endX, int endY) {
  Bridge* bridge = nullptr;
  std::vector<int> tmp_bridge;
  // contains all information the sub-class Bridge needs.
  tmp_bridge.resize(4);

  _start = isIsle(startX, startY);
  // checks whether the clicked point is on a isle and returns the coordinates
  // of the isle if it is.
  _end = isIsle(endX, endY);

  if (startX == -1 && startY == -1) { return; }
  // Don't create a bridge if it is the first click.

  if ((_start.first == 0 && _start.second == 0) ||
      (_end.first == 0 && _end.second == 0)) { return; }
  // Also return if one of the last to clicks isn't on an isle.

  if (_start.first == _end.first && _start.second == _end.second) { return; }
  // Return if both clicks are on the same isle.

  if (_start.first != _end.first && _start.second != _end.second) { return; }
  // We can only create vertical or horizontal bridges.

  if (blocked(_start, _end)) { return; }
  // Can't create a bridge over a third isle.

  tmp_bridge[0] = _start.first;
  tmp_bridge[1] = _start.second;
  tmp_bridge[2] = _end.first;
  tmp_bridge[3] = _end.second;

  int flag = 0;
  // The amount of bridges between the two given isles.
  flag = checkBridge(_start, _end);

  if (flag > 1) { return; }
  // Return if we already got two bridges between the isles.

  bridge = new Bridge();
  bridge->new_bridge(tmp_bridge);
  _bridges.push_back(bridge);
  if (_undo_bridges.size() < (unsigned)_undos) {
    // Prepare the vector vor the undo function.
    _undo_bridges.push_back(bridge);
  } else if (_undo_bridges.size() >= (unsigned)_undos) {
    _undo_bridges.erase(_undo_bridges.begin());
    _undo_bridges.push_back(bridge);
  }
  bridge = nullptr;

  _lastClickedX = -1;
  _lastClickedY = -1;
  countDownUp(_end, false);
  countDownUp(_start, false);
  // All done? So reset your latest clicks and countdown the value of the
  // both isles.

  drawBridge();
  // Draw the new state of the bridges.
}

// ____________________________________________________________________________
void Hashi::drawBridge() {
  std::pair<size_t, size_t> start;
  std::pair<size_t, size_t> end;
  for (auto& bridge : _bridges) {
    // We always draw every bridge new.
    // So after an undo we just have to call drawBridge() and the bridge
    // disappears.
    int flag = 0;
    start = bridge->getStart();
    end = bridge->getEnd();
    flag = checkBridge(start, end) - 1;
    if (flag > 1) { continue; }
    // Normally this should never happen, because we check it already
    // in the newBridge()-function.

    if (start.first == end.first) {
      // If we want to draw a horizontal bridge.
      if (start.second < end.second) {
        // And we we have to draw down.
        for (size_t i = start.second; i < end.second; i++) {
          if (flag == 1) {
            // Already exists one bridge.
            mvprintw(i, start.first, "H");
          } else if (flag == 0) {
            // The first bridge between the isles.
            mvprintw(i, start.first, "|");
          }
        }
      } else {
        for (size_t i = end.second; i < start.second; i++) {
          if (flag == 1) {
            mvprintw(i, start.first, "H");
          } else if (flag == 0) {
            mvprintw(i, start.first, "|");
          }
        }
      }
    } else if (start.second == end.second) {
      // If we want to draw a horizontal bridge.
      if (start.first < end.first) {
        for (size_t i = start.first; i < end.first; i++) {
          if (flag == 1) {
            mvprintw(start.second, i, "=");
          } else if (flag == 0) {
            mvprintw(start.second, i, "-");
          }
        }
      } else {
        for (size_t i = end.first; i < start.first; i++) {
          if (flag == 1) {
            mvprintw(start.second, i, "=");
          } else if (flag == 0) {
            mvprintw(start.second, i, "-");
          }
        }
      }
    }
  }
}

// ____________________________________________________________________________
int Hashi::checkBridge(std::pair<size_t, size_t> s,
                       std::pair<size_t, size_t> e) {
  int flag = 0;
  std::pair<size_t, size_t> tmp_start;
  std::pair<size_t, size_t> tmp_end;

  for (auto& brid : _bridges) {
    // Compare the new bridge to every already existing bridge.
    tmp_start = brid->getStart();
    tmp_end = brid->getEnd();
    if ((tmp_start.first == s.first &&
        tmp_start.second == s.second &&
        tmp_end.first == e.first &&
        tmp_end.second == e.second) ||
        (tmp_end.first == s.first &&
        tmp_end.second == s.second &&
        tmp_start.first == e.first &&
        tmp_start.second == e.second)) {
      flag++;
      // Count up for every similiar bridge.
    }
  }
  return flag;
}

// ____________________________________________________________________________
bool Hashi::blocked(std::pair<size_t, size_t> start,
                    std::pair<size_t, size_t> end) {
  if (start.first == end.first) {
    // same if and for clauses as already described at drawBridge()
    if (start.second < end.second) {
      for (size_t i = start.second + 1; i < end.second - 1; i++) {
        for (auto& isle : _isles) {
          if (isle->getX() == start.first && isle->getY() == i) {
            // if another isle is where the bridge should be built,
            // return true for 'yes, the way is blocked'.
            return true;
          }
        }
      }
    } else {
      for (size_t i = end.second + 1; i < start.second - 1; i++) {
        for (auto& isle : _isles) {
          if (isle->getX() == start.first && isle->getY() == i) {
            return true;
          }
        }
      }
    }
  } else if (start.second == end.second) {
    if (start.first < end.first) {
      for (size_t i = start.first + 1; i < end.first - 1; i++) {
        for (auto& isle : _isles) {
          if (isle->getX() == i && isle->getY() == start.second) {
            return true;
          }
        }
      }
    } else {
      for (size_t i = end.first + 1; i < start.first - 1; i++) {
        for (auto& isle : _isles) {
          if (isle->getX() == i && isle->getY() == start.second) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

// ____________________________________________________________________________
std::pair<size_t, size_t> Hashi::isIsle(size_t x, size_t y) {
  for (auto& isle : _isles) {
    // We compare our mouse click position to every existing isle.
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        // With this for loops, a 3x3 clicking window gets created.
        if (y+j == isle->getY() && x+i == isle->getX()) {
          return std::pair<size_t, size_t>(isle->getX(), isle->getY());
        }
      }
    }
  }
  return std::pair<size_t, size_t>(0, 0);
  // The function needs a return value.
}

// ____________________________________________________________________________
void Hashi::countDownUp(std::pair<size_t, size_t> s, bool plus) {
  for (auto& elem : _isles) {
    if (!plus) {
      // Everytime we build a bridge we count the value of the
      // involved isles one down.
      if (elem->getX() == s.first && elem->getY() == s.second) {
        elem->changeValue(false);
      }
    } else {
      // Otherwise we undo a bridge and count the value one up.
      if (elem->getX() == s.first && elem->getY() == s.second) {
        elem->changeValue(true);
      }
    }
  }
}

// ____________________________________________________________________________
void Hashi::undo() {
  if (_undo_bridges.empty()) { return; }
  // Nobody has start playing so we can't undo something.
  std::pair<size_t, size_t> start;
  std::pair<size_t, size_t> end;
  auto& erase = _bridges.back();
  // undo the latest built bridge.
  start = erase->getStart();
  end = erase->getEnd();
  if (start.first == end.first) {
    // Erase a vertical bridge.
    if (start.second < end.second) {
      for (size_t i = start.second; i < end.second; i++) {
        mvprintw(i, start.first, " ");
      }
    } else {
      for (size_t i = end.second; i < start.second; i++) {
        mvprintw(i, start.first, " ");
      }
    }
  } else if (start.second == end.second) {
    // erase a horizontal bridge.
    if (start.first < end.first) {
      for (size_t i = start.first; i < end.first; i++) {
        mvprintw(start.second, i, " ");
      }
    } else {
      for (size_t i = end.first; i < start.first; i++) {
        mvprintw(start.second, i, " ");
      }
    }
  }
  countDownUp(start, true);
  countDownUp(end, true);
  _undo_bridges.pop_back();
  _bridges.pop_back();
  drawBridge();
}

// ____________________________________________________________________________
int Hashi::victory() {
  int vic_flag = 1;
  for (auto& isle : _isles) {
    // All values are 0, you won.
    if (isle->getValue() != 0) {
      return vic_flag = 0;
    }
  }
  return vic_flag;
}

// ____________________________________________________________________________
void Hashi::solve(const std::string& filename) {
  // Open file and check whether it worked or not.
  std::ifstream file(filename.c_str());
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    endwin();
    exit(1);
  }

  std::string line;

  if (filename.substr(filename.find_last_of('.')) == ".solution") {
    // Looking for a *.solution file.
    while (true) {
      std::getline(file, line);
      if (line[0] == '#') { continue; }
      // Don't care about comments.
      if (file.eof()) { break; }
      // Stop as we reach the end of the file.
      size_t pos1 = line.find(",");
      size_t pos2 = line.find(",", pos1+1);
      size_t pos3 = line.find(",", pos2+1);

      _startIsleX = 5 * (stoi(line.substr(0, pos1)) + 1);
      _startIsleY = 5 * (stoi(line.substr(pos1 + 1, pos2 - pos1 - 1)) + 1);
      // Start isle.
      _lastClickedX = 5 * (stoi(line.substr(pos2 + 1, pos3 - pos2 - 1)) + 1);
      _lastClickedY = 5 * (stoi(line.substr(pos3 + 1)) + 1);
      // End isle.

      newBridge(_startIsleX, _startIsleY, _lastClickedX, _lastClickedY);
    }
  }
  if (victory() == 0) {
    // In case something didn't work out with the solution file.
    mvprintw(0, 0, "%s is the wrong solution file,", filename.c_str());
    mvprintw(1, 0, "or at least one bridge is built the wrong way.");
  }
}

// ____________________________________________________________________________
void Hashi::showState() {
  attron(A_REVERSE);
  for (auto& element : _isles) {
    color_set(2, 0);
    // Basic case: Draw white on black.
    if (element->getValue() == 0) {
      color_set(1, 0);
      // Isle has value = 0: Draw green on black.
    } else if (element->getValue() < 0) {
      color_set(3, 0);
      // Isle has to many bridges: Draw red on black.
    }
    printAround(element->getY(), element->getX());
    // Draw a 3x3 Isle.
    mvprintw(element->getY(), element->getX(), "%d", element->getValue());
  }
  color_set(2, 0);
  // Go back ino basic case.
  attroff(A_REVERSE);
}
