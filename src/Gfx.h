#pragma once

#include "Global.h"
#include "WorldListener.h"
#include <vector>

class World;

class Gfx : public WorldListener
{
public:
  
  Gfx(World *world_)
    : world(world_)
  {

  }

  virtual ~Gfx() { }
  
  void onFrame() override;
  void onAttack(const Unit &from, const Unit &to) override;
  void onKill(const Unit &killed) override;
  void onAttacksDone() override;
  void onGameEnd() override;

  void onDisplay();
  
  World *world;
  std::vector<Unit> attacks, killed;
  TimeStats displayStats; // display time stats
};
