// Copyright: Tom Krebs 2018
// Mail: <tomkre13@gmail.com>

#include <unistd.h>
#include <ncurses.h>
#include "./Hashi.h"

// ____________________________________________________________________________
int main(int argc, char** argv) {
  Hashi hashi;
  hashi.parseCommandLineArguments(argc, argv);
  hashi.initializeGame();
  refresh();
  hashi.play();
  // First we read in a given file with parseCommandLineArguments.
  // Then we convert the information from the file with initializeGame.
  // After that we only call our play function where we get caught
  // in a while loop, so we can play the game.
}
