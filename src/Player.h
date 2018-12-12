#pragma once

#include "Global.h"
#include "World.h"

class PlayerView;

class Player
{
public:
  
  Player()
    : world(nullptr), playerId(-1)
  {
  }

  virtual ~Player()
  {
  }
  
  void setup(World *world_, int playerId_, const std::string &name_,
             fp_t qtEps_,
             const std::string &policy_, int seed)
  {
    world = world_;
    playerId = playerId_;
    name = name_;
    qtEps = qtEps_;
    policy = policy_;
    rng.seed(seed);
  }

  void setId(int id) { playerId = id; }
  int getId() const { return playerId; }

  std::string getName() const { return name; }
  std::string getPolicy() const { return policy; }

  fp_t getQtEps() const { return qtEps; }
  
  PlayerView &self();
  const PlayerView &opponent() const;

  fp_t getWidth() const { return world->getWidth(); }
  fp_t getHeight() const { return world->getHeight(); }

  virtual void onFrame(int frameCount) = 0;
  virtual void onGameEnd() = 0;

  virtual void onBorderCollision(int /*unitId*/) { }

  // helpers

  // integer [0,n)
  int rndInt(int n) const { return world->rndInt(n, rng); }

  // double [0,1)
  fp_t rnd01() const { return world->rnd01(rng); }

  // random location for circle that fits
  Vec2 rndPos(fp_t radius = 0.0f) const { return world->rndPos(radius, rng); }
  
  // random location for circle that fits along edge
  Vec2 rndEdgePos(fp_t radius, const Vec2 &now) const
  {
    return world->rndEdgePos(radius, now, rng);
  }
  
private:

  World *world;
  int playerId;
  std::string name;
  fp_t qtEps;
  std::string policy;
  mutable RNG rng;
};
