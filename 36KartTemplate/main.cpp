#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "game/JXGame.hpp"
#undef main
int main(int ArgCount, char **Args)
{
  JXGame game;
  game.Run();
  return 0;
}