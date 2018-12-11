#include <iostream>
#include "PlayerView.h"
#include "P_IndCtrl.h"

using namespace std;

// returns policy name
string P_IndCtrl::policyToString(Policy pol)
{
  switch (pol) {

    case ATTACK_NONE:
      return "attack-none";
    
    case ATTACK_CLOSEST:
      return "attack-closest";
      
    case ATTACK_WEAKEST:
      return "attack-weakest";
      
    case ATTACK_MOST_DANGEROUS:
      return "attack-most-dangerous";
  }

  ERR("IndCtrlPlayer: unknown policy " << pol);
}

P_IndCtrl::Policy P_IndCtrl::policyFromString(const string &ps)
{
  if (ps == "attack_none") {
    return ATTACK_NONE;
  } else if (ps == "attack_closest") {
    return ATTACK_CLOSEST;
  } else if (ps == "attack_weakest") {
    return ATTACK_WEAKEST;
  } else if (ps == "attack_most_dangerous") {
    return ATTACK_MOST_DANGEROUS;
  }
  
  ERR("P_IndCtrl: unknown policy '" << ps << "'");
  return ATTACK_NONE;
}

void P_IndCtrl::onFrame(int /*frameCount*/)
{
#if DEBUG_PRINT
  
  cout << "frame " << frameCount << endl;

  cout << "my units" << endl;

  for (auto &u : self().getUnits()) {
    cout << u.unitId << " " << u.owner << " - ";
  }

  cout << "your units" << endl;

  for (auto &u : opponent().getUnits()) {
    cout << u.unitId << " " << u.owner << " - ";
  }

  cout << endl;
#endif

  Quadtree<Unit> qtOpp;
  bool useQt = Quadtree<Unit>::useQt(getQtEps());
  
  if (useQt) {

    // larger to accomodate max coordinates
    qtOpp.setup(self().getWidth()+1, self().getHeight()+1, getQtEps());

    for (auto &u : opponent().getUnits()) {
      qtOpp.insert(&u);
    }
  }
  
  for (auto &u : self().getUnits()) {

    if (u.readyForAttack()) {

      // ready to attack

      vector<int> targetIds;
      vector<const Unit *> attackableUnits;

      if (useQt) {
        self().enemiesWithinAttackRange(u, qtOpp, opponent().getMaxRadius(), attackableUnits);
      } else {
        self().enemiesWithinAttackRange(u, opponent().getUnits(), attackableUnits);
      }

      // cout << "targets " << attackableUnits.size() << endl;
      
      switch (polEnum) {

        case ATTACK_NONE:
          // could be faster by skipping above code
          // but want to measure performance of above without decimating units
          break;
        
        case ATTACK_WEAKEST:
          self().weakestTargetIndexes(u, attackableUnits, targetIds);
          break;
          
        case ATTACK_CLOSEST:
          self().closestTargetIndexes(u, attackableUnits, targetIds);
          break;
          
        case ATTACK_MOST_DANGEROUS:
          self().mostDangerousTargetIndexes(u, attackableUnits, targetIds);
          break;
          
        default:
          cerr << "P_IndCtrl: unknown policy" << endl;
          exit(10);
      }

      // cout << targetIds.size() << endl;
      
      if (!targetIds.empty()) {

        // attack

        Action a;
        a.type = Action::ATTACK;
        a.attackTargetId = targetIds[0];

        // generate attack action
        self().addAction(u, a);
        DPRINT("player: attack " << u.unitId << " target " << a.attackTargetId);
        continue;
      }
    }
        
    if (!u.isMoving()) {

      // new move location
      
      DPRINT("move");

      Action a;
      a.type = Action::MOVE;
      a.movePos = rndEdgePos(u.radius, u.pos);
      self().addAction(u, a);

      DPRINT("player: move " << u.unitId << " to " << a.movePos);
    }
  }
}

void P_IndCtrl::onGameEnd()
{
  cout << "game ended" << endl;
}
