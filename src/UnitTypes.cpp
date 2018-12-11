#include "UnitTypes.h"

// data from https://liquipedia.net/starcraft/List_of_Unit_and_Building_Sizes
// etc.

Unit makeMarine(int owner_, const Vec2 &pos)
{
  Unit u(owner_, pos);

  u.type = MARINE;

  u.radius = 9;
  u.maxSpeed = 4;
  u.visionRange = 7*32;
  u.attackRange = 4*32;
  u.maxHp = 40;
  u.attack = 6;
  u.cooldown = 15;
  u.onlyAttackWhenStopped = true;

  u.startValues();

  return u;
}

Unit makeTank(int owner_, const Vec2 &pos)
{
  Unit u(owner_, pos);

  u.type = TANK;

  u.radius = 16;
  u.maxSpeed = 4;
  u.visionRange = 10*32;
  u.attackRange = 7*32;
  u.maxHp = 150;
  u.attack = 30;
  u.cooldown = 37;
  u.onlyAttackWhenStopped = true;

  u.startValues();

  return u;
}
