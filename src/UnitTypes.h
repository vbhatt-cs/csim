#include "Unit.h"

enum UnitTypes { MARINE = 0, TANK };

Unit makeMarine(int owner_, const Vec2 &pos = Vec2());

Unit makeTank(int owner_, const Vec2 &pos = Vec2());

// todo: implement siege mode (including splash and minimum attack range)
//       and other unit types ...
