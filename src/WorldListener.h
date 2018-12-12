#pragma once

struct Unit;

struct WorldListener
{
  virtual void onFrame() = 0;
  virtual void onAttack(const Unit &from, const Unit &to) = 0;
  virtual void onKill(const Unit &killed) = 0;
  virtual void onAttacksDone() = 0;
  virtual void onGameEnd() = 0;
};
