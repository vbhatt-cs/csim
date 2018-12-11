#pragma once

#include <set>
#include "Global.h"
#include <iostream>

struct Unit
{
  static int id; // increases with every constructor call
  
  Unit(int owner_ = -1, const Vec2 &pos_ = { 0, 0 })
    : owner(owner_), pos(pos_)
  {
    radius = 0;
    maxSpeed = 0;
    visionRange = 0;
    attackRange = 0;
    maxHp = 0;
    attack = 0;
    cooldown = 0;
    onlyAttackWhenStopped = true;

    unitId = ++id;
    type = -1;

    alive = true;

    delta = {0,0};
    targetPos = {0,0};
    moveCount = 0;
    
    hp = 0;
    cooldownCount = 0;

    startValues();
  }
  
  // fixed unit type properties
  fp_t radius;       // pixels
  fp_t maxSpeed;     // pixels/frame
  fp_t visionRange;  // pixels
  fp_t attackRange;  // pixels
  int    maxHp;
  int    attack;
  int    cooldown;     // frames to wait before attacking next (0 = constant firing)
  bool   onlyAttackWhenStopped;
  
  // static
  int    unitId;
  int    type;
  int    owner;

  // dynamic
  bool   alive;
  Vec2   pos;

  Vec2   delta;      // position change per tick
  Vec2   targetPos;
  int    moveCount; // = 0 <=> stop

  int    hp;
  int    cooldownCount;  // <= 0 <=> can attack

  fp_t getX() const
  {
    return pos.x;
  }
  
  fp_t getY() const
  {
    return pos.y;
  }
  
  void startValues()
  {
    alive = true;
    hp = maxHp;
    cooldownCount = 0;
    stopMotion();
  }
  
  void stopMotion()
  {
    moveCount = 0;
  }

  bool isMoving() const
  {
    return moveCount > 0;
  }
  
  void startMotion(const Vec2 &whereTo)
  {
    DPRINT("world: start motion " << unitId);
    
    fp_t speed = maxSpeed; // future: pass on as parameter?
    fp_t d = static_cast<fp_t>(sqrt(whereTo.dist2(pos)));
    fp_t time = d / speed;

    if (time >= 100'000) {
      std::cerr << "motion too slow" << std::endl;
      exit(10);
    }
    
    moveCount = static_cast<int>(ceil(time));

    if (!moveCount) {
      return;
    }

    assert(time > 0);    
    
    targetPos = whereTo;
    delta = whereTo.sub(pos);
    delta.scale(static_cast<fp_t>(1.0/time));
  }

  bool readyForAttack() const
  {
    return cooldownCount <= 0;
  }

  // @return true if uTo is dead
  bool executeAttack(Unit &uTo, int cooldownDelta)
  {
    assert(readyForAttack());
    uTo.hp -= attack;
    // randomize cooldown -1..+2
    cooldownCount = (cooldown+1)+cooldownDelta; // note: +1 because first tick happens at end of this frame
    cooldownCount = std::max(cooldownCount, 1);
    return uTo.hp <= 0;
  }
  
  void tick()
  {
    DPRINT("world: tick " << unitId << " " << moveCount << " " << cooldownCount << " " << isMoving());

    if (isMoving()) {
      DPRINT("move tick");
      --moveCount; 
   }

    if (!readyForAttack()) {
      --cooldownCount;
      DPRINT("cooldown tick foo");
    }

    DPRINT("world: tick2" << unitId << " " << moveCount << " " << cooldownCount << " " << isMoving())
  }
  
};

inline std::ostream &operator<<(std::ostream &os, const Unit &u)
{
  os << "id " << u.unitId << " x " << u.pos.x << " y " << u.pos.y << " r " << u.radius
     << " v " << u.visionRange << " hp " << u.hp;
  return os;
}

inline bool operator<(const Unit &u, const Unit &v)
{
  return u.unitId < v.unitId;
}
