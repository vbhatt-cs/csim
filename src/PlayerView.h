#pragma once

// world interface for players
// what a player sees ...

#include "Unit.h"
#include "Action.h"
#include "Quadtree.h"
#include <vector>
#include <map>

class PlayerView
{
public:
  
  int getId() const { return playerId; }

  std::string getName() const { return std::string("player") + std::to_string(playerId); }

  const std::vector<Unit> &getUnits() const { return units; }

  fp_t getHeight() const { return height; }
  fp_t getWidth() const { return width; }
  
  fp_t getMaxRadius() const { return maxRadius; }

  void addAction(const Unit &actor, const Action &action)
  {
    actions.insert({ actor.unitId, action });
  }

  void enemiesWithinAttackRange(const Unit &u,
                                const std::vector<Unit> &enemyUnits,
                                std::vector<const Unit *> &attackableUnits) const;

  void enemiesWithinAttackRange(const Unit &u,
                                const Quadtree<Unit> &qtOpp,
                                float4 maxRadius,
                                std::vector<const Unit *> &attackableUnits) const;

  void closestTargetIndexes(const Unit &u,
                            const std::vector<const Unit *> &attackableUnits,
                            std::vector<int> &targetIds) const;

  void weakestTargetIndexes(const Unit &u,
                            const std::vector<const Unit *> &attackableUnits,
                            std::vector<int> &targetIds) const;

  void mostDangerousTargetIndexes(const Unit &u,
                                  const std::vector<const Unit *> &attackableUnits,
                                  std::vector<int> &targetIds) const;
private:

  // todo: race? upgrades? ...

  fp_t width, height;
  bool fogOfWar;
  fp_t qtEps;
  int playerId;
  std::vector<Unit> units;
  float4 maxRadius; // of all units (for quadtree queries)
  std::map<int, Action> actions; // unit id -> action

  // called by world
  
  void setup(int playerId_, fp_t width_, fp_t height_, bool fogOfWar_, fp_t qtEps_)
  {
    playerId = playerId_;
    width = width_;
    height = height_;
    fogOfWar = fogOfWar_;
    qtEps = qtEps_;
    units.clear();
    maxRadius = 0;
    actions.clear();
  }

  void addUnit(const Unit &u)
  {
    units.push_back(u);
    maxRadius = std::max(maxRadius, u.radius);
  }

  friend class World;
};


// from BWAPI: future extensions ...

// Race getRace() const;
// PlayerType getType() const;
// Force getForce() const;
// bool isAlly(const Player player) const;
// bool isEnemy(const Player player) const;
// bool isNeutral() const;

// TilePosition getStartLocation() const;

// bool isVictorious() const;

// bool isDefeated() const;

// bool leftGame() const;

// int minerals() const = 0;
// int gas() const = 0;
// int gatheredMinerals() const = 0;  // or 0 if inaccessible
// int gatheredGas() const = 0;
// int repairedMinerals() const = 0;
// int repairedGas() const = 0;
// int refundedMinerals() const = 0;
// int refundedGas() const = 0;
// int spentMinerals() const = 0;
// int spentGas() const = 0;
// int supplyTotal(Race race = Races::None) const = 0;
// int supplyUsed(Race race = Races::None) const = 0;
// int allUnitCount(UnitType unit = UnitTypes::AllUnits) const = 0;
// int visibleUnitCount(UnitType unit = UnitTypes::AllUnits) const = 0;
// int completedUnitCount(UnitType unit = UnitTypes::AllUnits) const = 0;
// int incompleteUnitCount(UnitType unit = UnitTypes::AllUnits) const;
// int deadUnitCount(UnitType unit = UnitTypes::AllUnits) const = 0;
// int killedUnitCount(UnitType unit = UnitTypes::AllUnits) const = 0;
// int getUpgradeLevel(UpgradeType upgrade) const = 0;
// bool hasResearched(TechType tech) const = 0;
// bool isResearching(TechType tech) const = 0;
// bool isUpgrading(UpgradeType upgrade) const = 0;
// BWAPI::Color getColor() const = 0;
// char getTextColor() const;

// int maxEnergy(UnitType unit) const;
// double topSpeed(UnitType unit) const;
// int weaponMaxRange(WeaponType weapon) const;
// int sightRange(UnitType unit) const;
// int weaponDamageCooldown(UnitType unit) const;
// int armor(UnitType unit) const;
// int damage(WeaponType wpn) const;

// int getUnitScore() const = 0;
// int getKillScore() const = 0;
// int getBuildingScore() const = 0;
// int getRazingScore() const = 0;
// int getCustomScore() const = 0;

// bool isObserver() const = 0;
// int getMaxUpgradeLevel(UpgradeType upgrade) const = 0;
// bool isResearchAvailable(TechType tech) const = 0;
// bool isUnitAvailable(UnitType unit) const = 0;
// bool hasUnitTypeRequirement(UnitType unit, int amount = 1) const;
