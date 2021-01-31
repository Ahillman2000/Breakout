#include "game.h"
int main()
{
  Breakout asge_game;
  if (asge_game.init())
  {
    asge_game.run();
  }
  return 0;
}