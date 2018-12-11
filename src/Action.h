#pragma once

struct Action
{
  enum Type { NOP = 0, ATTACK, MOVE, STOP };

  Action()
  {
    type = NOP;
  }
  
  Type type;
  
  int attackTargetId;

  Vec2 movePos;
};
