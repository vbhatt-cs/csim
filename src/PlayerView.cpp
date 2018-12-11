#include "PlayerView.h"

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include "World.h"
#include "Unit.h"

using namespace std;

// populate a vector with enemy units that can be attacked by u;
// clears vector first
void PlayerView::enemiesWithinAttackRange(const Unit &u,
                                          const vector<Unit> &enemyUnits,
                                          vector<const Unit*> &attackableUnits) const
{
  attackableUnits.clear();

  for (size_t i=0; i < enemyUnits.size(); ++i) {
    const Unit &v = enemyUnits[i];
    
    if (u.owner == v.owner) {
      cout << "what?" << endl;
      continue; // player is owner
    }

    if (World::canAttack(u, v)) {
      attackableUnits.push_back(&v);
    }
  }
}


void PlayerView::enemiesWithinAttackRange(const Unit &u,
                                          const Quadtree<Unit> &qtOpp,
                                          float4 maxRadius,
                                          std::vector<const Unit *> &attackableUnits) const
{
  assert(maxRadius > 0);
  
  float4 r = u.visionRange + maxRadius * 1.1f; // avoid rounding problems
  vector<const Unit*> candidateUnits;

  // cout << "enemy query " << u.unitId << endl;
  
  qtOpp.query(u.pos.x - r, u.pos.x + r, u.pos.y - r, u.pos.y + r, candidateUnits);
  
  attackableUnits.clear();

  for (size_t i=0; i < candidateUnits.size(); ++i) {
    const Unit &v = *candidateUnits[i];
    
    if (u.owner == v.owner) {
      cout << "what?" << endl;
      continue; // player is owner
    }

    if (World::canAttack(u, v)) {
      attackableUnits.push_back(&v);
    }
  }
}


// return a random unit that can be attacked by u with minimal hp_old value,
// or 0 if none exists

void PlayerView::weakestTargetIndexes(const Unit &/*u*/,
                                      const vector<const Unit*> &attackableUnits,
                                      vector<int> &targetIds) const
{
  targetIds.clear();
  
  int n = (int)attackableUnits.size();

  if (!n) {
    return;
  }

  int min = numeric_limits<int>::max();
  
  for (const Unit *p : attackableUnits) {

    const Unit &tu = *p;
    int hp = tu.hp;

    if (hp > min) {
      continue;
    }
    
    if (hp < min) {
      min = hp;
      targetIds.clear();
    }

    targetIds.push_back(tu.unitId);
  }

  assert(!targetIds.empty());  
}


// return a random unit that can be attacked by u with minimal distance to u,
// or 0 if none exists

void PlayerView::closestTargetIndexes(const Unit &u,
                                      const vector<const Unit*> &attackableUnits,
                                      vector<int> &targetIds) const
{
  targetIds.clear();
  
  int n = (int)attackableUnits.size();

  if (!n) {
    return;
  }

  double min = std::numeric_limits<double>::max();
  
  for (const Unit *p : attackableUnits) {

    const Unit &tu = *p;
    double d2 = u.pos.dist2(tu.pos);

    if (d2 > min) {
      continue;
    }
    
    if (d2 < min) {
      min = d2;
      targetIds.clear();
    }

    targetIds.push_back(tu.unitId);
  }

  assert(!targetIds.empty());
}


// return a random unit that can be attacked by u with maximal damage/hp_old
// ratio, or 0 if none exists

void PlayerView::mostDangerousTargetIndexes(const Unit &/*u*/,
                                      const vector<const Unit*> &attackableUnits,
                                      vector<int> &targetIds) const
{
  targetIds.clear();
  
  int n = (int)attackableUnits.size();

  if (!n) {
    return;
  }

  double max = std::numeric_limits<double>::lowest();
  
  for (const Unit *p : attackableUnits) {

    const Unit &tu = *p;

    assert(tu.hp > 0);
    double danger = (double) tu.attack / (double) (tu.cooldown+1) / tu.hp;

    if (danger < max) {
      continue;
    }

    if (danger > max) {
      max = danger;
      targetIds.clear();
    }

    targetIds.push_back(tu.unitId);
  }
  
  assert(!targetIds.empty());
}

