// Copyright: Tom Krebs 2018
// Mail: <tomkre13@gmail.com>

#include <gtest/gtest.h>
#include "./Hashi.h"

// ____________________________________________________________________________
TEST(HashiTest, setUndoDefault) {
  Hashi hashi;
  int argc = 2;
  char* argv[2] = {
    const_cast<char*>("./HashiMain"),
    const_cast<char*>("'/instances/i001-n002-s03x01.plain'")
  };
  hashi.parseCommandLineArguments(argc, argv);
  ASSERT_EQ(hashi._undos, 5);
}

// ____________________________________________________________________________
TEST(HashiTest, parseCommandLineArguments) {
  Hashi hashi;
  int argc = 4;
  char* argv[4] = {
    const_cast<char*>("./HashiMain"),
    const_cast<char*>("--undo"),
    const_cast<char*>("15"),
    const_cast<char*>("'/instances/i001-n002-s03x01.plain'")
  };
  hashi.parseCommandLineArguments(argc, argv);
  ASSERT_EQ(15, hashi._undos);
}
