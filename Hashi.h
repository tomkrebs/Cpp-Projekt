// Copyright: Tom Krebs 2018
// Mail: <tomkre13@gmail.com>


#ifndef HASHI_H_
#define HASHI_H_


#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <map>
#include <vector>
#include "./Object.h"

class Hashi {
 public:
  // Constructor.
  Hashi();

  // Deconstructor.
  ~Hashi();

  // Print out the correct use of the command line.
  void printUsageAndExit() const;

  // Parse the command line arguments.
  void parseCommandLineArguments(int argc, char** argv);

  // Initialize the game.
  void initializeGame();

  // Prints around (x, y) to create a 3x3 isle.
  void printAround(size_t y, size_t x);

  // The while (true) - loop, which keeps the game running.
  void play();

  // Gets the users input and converts it into an action.
  void processUserInput(int key);

  // Creates a bridge between two isles.
  void newBridge(int startX, int startY, int endX, int endY);

  // Returns true if a Isle is in the middle of to other isles you
  // wanted to connect with a bridge.
  bool blocked(std::pair<size_t, size_t> start, std::pair<size_t, size_t> end);

  // Checks whether the clicked position is on an isle or not.
  std::pair<size_t, size_t> isIsle(size_t x, size_t y);

  // Decrements _value, when a bridge was build.
  // And counts up _value if the user wants to undo a bridge.
  // Plus is true for addition and false for subtraction.
  void countDownUp(std::pair<size_t, size_t> s, bool plus);

  // Draws the bridges.
  void drawBridge();

  // Check whether we can build another bridge on that isle or not.
  // Returns the amount of bridges between to isles.
  int checkBridge(std::pair<size_t, size_t> s, std::pair<size_t, size_t> e);

  // Undos a bridge (max. _undos times)
  void undo();

  // Checks whether the Hashi is solved.
  int victory();

  // Tries to solve the given Hashi with the instructions from filename.
  void solve(const std::string& filename);

  // Function for showing the actual state in the window.
  void showState();

 private:
  FRIEND_TEST(HashiTest, setUndoDefault);
  FRIEND_TEST(HashiTest, parseCommandLineArguments);
  FRIEND_TEST(HashiTest, setUndoWrongUsage);

  // That's how often we can undo something.
  int _undos;

  // The File Name.
  std::string _inputFileName;

  // The Name of the solution file.
  std::string _inputSolutionFileName;

  // Position of the latest click.
  int _lastClickedX = -1;
  int _lastClickedY = -1;

  // Position of the second latest click.
  int _startIsleX = 0;
  int _startIsleY = 0;

  // Start and end coordinates of a bridge.
  std::pair<size_t, size_t> _start;
  std::pair<size_t, size_t> _end;

  // A vector which contains all actuall isles.
  std::vector<Isle*> _isles;

  // A vector which contains all actuall bridges.
  std::vector<Bridge*> _bridges;

  // A vector which contains all the bridges we can undo in a row.
  std::vector<Bridge*> _undo_bridges;
};

#endif  // HASHI_H_
