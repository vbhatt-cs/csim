#include "World.h"
#include "Player.h"
#include <set>
#include <iostream>
#include <array>

using namespace std;

// todo: use quadtrees

void World::setup(fp_t width_, fp_t height_, bool fogOfWar_, int seed,
                  Player *p0, Player *p1,
                  fp_t qtEps_,
                  const std::string &params_)
{
  width = width_;
  height = height_;
  fogOfWar = fogOfWar_;
  qtEps = qtEps_;

  // larger to accomodate max coordinates
  //!!!qts[0].setup(width+1, height+1, qtEps);
  //!!!qts[1].setup(width+1, height+1, qtEps);  
  
  players.clear();
  players.push_back(p0);
  players.push_back(p1);
  p0->setId(0);
  p1->setId(1);

  params = params_;
  
  id2unit.clear();

  rng.seed((unsigned long)seed);
}

// integer [0,n)
int World::rndInt(int n, RNG &rng) const
{
  assert(n > 0);
  std::uniform_int_distribution<int> dist(0,n-1); // range [0,n-1]
  int r = dist(rng);
  assert(r >= 0 && r < n);
  return r;
}

// double [0,1)
fp_t World::rnd01(RNG &rng) const
{
  std::uniform_real_distribution<double> dist(0,1); // range [0,1)
  double r = dist(rng);
  assert(r >= 0 && r < 1);
  return (fp_t)r;
}

Vec2 World::rndPos(fp_t radius, RNG &rng) const
{
  fp_t slack = radius * 1.1f;

  Vec2 p((width  - 2*slack) * rnd01(rng) + slack,
         (height - 2*slack) * rnd01(rng) + slack);
  
  // assert circle in rectangle
  assert(p.x > radius && p.x < width - radius);
  assert(p.y > radius && p.y < height - radius);
  return p;
}

Vec2 World::rndEdgePos(fp_t radius, const Vec2 &now, RNG &rng) const
{
  fp_t slack = radius * 1.05f;
  fp_t xLow = width  * 0.05f, xHigh = width  * 0.95f;
  fp_t yLow = height * 0.05f, yHigh = height * 0.95f;
  Vec2 p;

  if (rnd01() < 0.5f) {
    
    // constrain y
    
    p.x = (width - 2*slack) * rnd01() + slack;

    if (now.y < yLow) {
      p.y = height - slack;
    } else if (now.y > yHigh) {
      p.y = slack;
    } else if (rnd01() < 0.5f) {
      p.y = slack;
    } else {
      p.y = height - slack;
    }
    
  } else {
    
    // constain x

    p.y = (height - 2*slack) * rnd01(rng) + slack;

    if (now.x < xLow) {
      p.x = width - slack;
    } else if (now.x > xHigh) {
      p.x = slack;
    } else if (rnd01() < 0.5f) {
      p.x = slack;
    } else {
      p.x = width - slack;
    }
  }
    
  // assert circle in rectangle
  assert(p.x > radius && p.x < width - radius);
  assert(p.y > radius && p.y < height - radius);
  return p;
}

pair<int, int> World::countUnits() const
{
  pair<int, int> counts{ 0, 0 };

  for (auto &p : id2unit) {
    if (p.second.owner == 0) {
      ++counts.first;
    } else {
      ++counts.second;
    }
  }
  return counts;
}


/*
  f  f.vision   t.r t
  x------------|----x
  |                 |
       d(f, t)

  f can see t iff f.visionRange > d(f, t) - t.radius
            
*/
            
bool World::canSee(const std::vector<Unit> &units, const Unit &u)
{
  // todo: speed up with sectors
  
  for (auto &v : units) {
    if (v.pos.dist2(u.pos) < square(v.visionRange + u.radius)) {
      return true;
    }
  }
  return false;
}


bool World::isVisible(const Unit &u)
{
  return canSee(selfViews[1-u.owner].getUnits(), u);
}


/*
  v  v.attack   u.r u
  x------------|----x
  |                 |
       d(v, u)

  v can attack u iff v.attackRange > d(v, u) - u.radius
            
*/
bool World::canAttack(const Unit &from, const Unit &to)
{
  return from.pos.dist2(to.pos) < square(from.attackRange + to.radius);
}


void World::computeViews()
{
  assert(players.size() == 2);

  selfViews.resize(2);
  opponentViews.resize(2);

  for (int i=0; i < 2; ++i) {
    selfViews[i].setup(i, width, height, fogOfWar, qtEps);
    opponentViews[i].setup(i, width, height, fogOfWar, qtEps);
    //!!!qts[i].clear();
  }  
  
  for (const auto &p : id2unit) {
    int ownerIndex = static_cast<size_t>(p.second.owner);
    selfViews[ownerIndex].addUnit(p.second);
    if (Quadtree<Unit>::useQt(qtEps)) {
      //!!!qts[ownerIndex].insert(&p.second);
    }
  }

  if (fogOfWar) {

    ERR("fow not tested");

    // todo: optionally use qt
    
    for (const auto &p : id2unit) {
      if (canSee(selfViews[static_cast<size_t>(1-p.second.owner)].getUnits(), p.second)) {
        opponentViews[static_cast<size_t>(p.second.owner)].addUnit(p.second);
      }
    }

  } else {

    // no fog of war
    opponentViews[0] = selfViews[1];
    opponentViews[1] = selfViews[0];
  }
}

using MapIterType = map<int,Unit>::iterator;

// compare map iterators (by address they point to)
bool operator<(const MapIterType &a, const MapIterType &b)
{
  return &*a < &*b;
}

void World::executeAttacks()
{
  Timer start;
  
  set<int> killed; // ids of killed units
  
  for (auto &p : players) {

    for (auto &a : p->self().actions) {

      Action act = a.second;

      if (act.type != Action::ATTACK) {
        continue;
      }

      int fromId = a.first;
      int toId = act.attackTargetId;
      
      auto fromIt = id2unit.find(fromId);
      
      if (fromIt == end(id2unit)) {
        DPRINT("attack: unknown from id " << fromId);
        continue;
      }

      Unit &uFrom = fromIt->second;
      
      if (uFrom.owner != p->self().playerId) {
        DPRINT("attack: not from owner " << fromId);
        continue;
      }

      if (!uFrom.readyForAttack()) {
        DPRINT("attack: can't attack yet " << fromId);
        continue;
      }

      auto toIt = id2unit.find(toId);

      if (toIt == end(id2unit)) {
        DPRINT("attack: unknown to id " << toId);
        continue;
      }

      Unit &uTo = toIt->second;

      if (uTo.owner == p->self().playerId) {
        DPRINT("attack: can't self-attack " << toId);
        continue;
      }

      if (!canAttack(uFrom, uTo)) {
        DPRINT("attack: can't attack " << fromId << " " << toId);
        continue;
      }

      // check visibility

      if (!isVisible(uTo)) {
        DPRINT("attack: not visible " << toId);
        continue;
      }

      // attack permitted

      DPRINT("world: attack " << uFrom.unitId << " target " << uTo.unitId);
      
      if (uFrom.executeAttack(uTo, rndInt(4)-1)) {
        killed.insert(uTo.unitId);
      }
        
      if (worldListener) { worldListener->onAttack(uFrom, uTo); }
      
      if (uFrom.onlyAttackWhenStopped) {
        uFrom.stopMotion();
      }
    }
  }

  // remove dead objects

  for (auto id : killed) {
    if (worldListener) { worldListener->onKill(id2unit.find(id)->second); }
    id2unit.erase(id);
  }

  Timer end;
  
  if (worldListener) { worldListener->onAttacksDone(); }
}


void World::moveUnit(Unit &u)
{
  if (!u.isMoving()) {
    return;
  }

  DPRINT("world: moving unit " << u.unitId);
  
  fp_t dx = u.delta.x;
  fp_t dy = u.delta.y;

  if (u.moveCount == 1) {
    // last step
    dx = u.targetPos.x - u.pos.x;
    dy = u.targetPos.y - u.pos.y;

    DPRINT("world: unit " << u.unitId << " about to stop");
  }
  
  fp_t t = 1;
  fp_t newX = u.pos.x + dx;
  fp_t newY = u.pos.y + dy;
  
  //bool collision[4] = { false, false, false, false };

  //cout << u.pos.x << " " << u.pos.y << " - " << new_x << " " << new_y;

  // compute collision time t <= 1
  
  if (dx < 0 && newX - u.radius < 0) {
    // cout << " LEFT " << newX - u.radius << endl;
    //collision[LEFT] = true;
    t = std::min(t, 1.0f-(newX - u.radius) / dx);
  }
  if (dx > 0 && newX + u.radius > width) {
    // cout << " RIGHT " << newX + u.radius - width << endl;
    //collision[RIGHT] = true;
    t = std::min(t, 1.0f-(newX + u.radius - width) / dx);
  }
  if (dy < 0 && newY - u.radius < 0) {
    //cout << " TOP " << newY - u.radius << endl;
    //collision[TOP] = true;
    t = std::min(t, 1.0f-(newY - u.radius) / dy);
  }
  if (dy > 0 && newY + u.radius > height) {
    //cout << " BOTTOM " << newY + u.radius - height << endl;
    //collision[BOTTOM] = true;
    t = std::min(t, 1.0f-(newY + u.radius - height) / dy);
  }

  // cout << " t=" << t;
  
  u.pos.x += dx * t;
  u.pos.y += dy * t;

  // clip to box

  if (u.pos.x - u.radius < 0     ) { u.pos.x = u.radius; }
  if (u.pos.x + u.radius > width ) { u.pos.x = width - u.radius; }
  if (u.pos.y - u.radius < 0     ) { u.pos.y = u.radius; }
  if (u.pos.y + u.radius > height) { u.pos.y = height - u.radius; }

  if (t < 1) {
    // border collision
    u.stopMotion();
    DPRINT("world: border collision " << u.unitId);
    
  } else {

    DPRINT("world: move step " << u.unitId << " " << (dx*t) << " " << (dy*t) << " mc " << u.moveCount);
  }
}


void World::executeMotion()
{
  Timer start;
      
  for (auto &p : players) {

    for (auto &a : p->self().actions) {

      auto it = id2unit.find(a.first);

      if (it == end(id2unit)) {
        continue;  // unit gone
      }

      Unit &u = it->second;
      Action act = a.second;

      if (u.owner != p->getId()) {
        cerr << "unit not owned by player" << endl;
        continue;
      }
      
      if (act.type == Action::MOVE) {

        u.startMotion(act.movePos);

        DPRINT("world: exec motion " << u.unitId << " to " << act.movePos);
        
      } else if (act.type == Action::STOP) {

        u.stopMotion();

        DPRINT("world: stop motion " << u.unitId);
      }
    }
  }

  // move all units
  
  for (auto &p : id2unit) {
    moveUnit(p.second);
  }

  // cooldown and motion ticks
  for (auto &p : id2unit) {
    p.second.tick();
  }
  
  Timer end;
  motionStats.update(end.diff(start));
}

bool World::executeFrame()
{
  // compute views

  if (getFrameCount() == 0) {
    startTime.set();
  }
  
  {
    Timer start;

    computeViews();

    Timer end;
    viewStats.update(end.diff(start));
  }
  
  // check game end condition
  
  if (gameFinished()) {
    if (worldListener) { worldListener->onGameEnd(); }
    players[0]->onGameEnd();
    players[1]->onGameEnd();
    return false;
  }

  if (worldListener) { worldListener->onFrame(); }
  
  // run player code

  for (int i=0; i < 2; ++i) {
    Timer start;

    players[i]->onFrame(frameCounter);

    Timer end;
    playerStats[i].update(end.diff(start));
  }

  // execute actions

  // attacks
  executeAttacks();
    
  // move units
  executeMotion();

  ++frameCounter;
  return true;
}

void World::writeStats() const
{
  cout << "frame " << getFrameCount() << " units " << id2unit.size();
  if (getFrameCount() > 0) {
    Timer now(Timer::WALLCLOCK);
    cout << " fps " << getFrameCount() / ((double)now.diff(startTime) / 1'000'000.0);
  }
  cout << endl;
  cout << "view millis: " << viewStats.avgMillis() << endl;
  cout << "action millis: " << actionStats.avgMillis() << endl;
  cout << "attack millis: " << attackStats.avgMillis() << endl;
  cout << "motion millis: " << motionStats.avgMillis() << endl;
  cout << "player " << players[0]->getName() << " millis: " << playerStats[0].avgMillis() << endl;
  cout << "player " << players[1]->getName() << " millis: " << playerStats[1].avgMillis() << endl;  
}
