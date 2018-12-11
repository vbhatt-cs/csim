#pragma once

#include <vector>
#include <map>
#include "PlayerView.h"
#include "WorldListener.h"
#include "Unit.h"
#include "Quadtree.h"

class Player;

class World
{
public:

  // player ids
  static constexpr int RED  = 0;
  static constexpr int BLUE = 1;

  World()
    : startTime(Timer::WALLCLOCK)
  {
    width = height = 0;
    fogOfWar = false;
    frameCounter = 0;
  }

  virtual ~World()
  {
  }

  // execute one simulation frame
  // @return false if game is over
  bool executeFrame();

  // setup world
  // qtEps: cell split dimension of quadtree (0: don't use qt)
  virtual void setup(fp_t width_, fp_t height_, bool fogOfWar_, int seed_,
                     Player *p0, Player *p1,
                     fp_t qtEps,
                     const std::string &params);
  
  virtual bool gameFinished() const = 0;

  virtual int score0(bool timeout) const = 0;

  // called when unit gets attacked by another (e.g. for animating attacks in a GUI)
  // virtual void onAttack(const Unit &/*from*/, const Unit &/*to*/) { } 

  void setListener(WorldListener *wl)
  {
    worldListener = wl;
  }
  
  PlayerView &getSelfView(int playerId)
  {
    return selfViews[static_cast<size_t>(playerId)];
  }

  const PlayerView &getOpponentView(int playerId)
  {
    return opponentViews[static_cast<size_t>(playerId)];
  }

  Vec2 mirror(const Vec2 &pos) const
  {
    return Vec2(width-pos.x, height-pos.y);
  }

  // random integer [0,n)
  int rndInt(int n) const { return rndInt(n, rng); }

  // random double [0,1)
  fp_t rnd01() const { return rnd01(rng); }

  // return a random position at which a circle of that radius fits
  Vec2 rndPos(fp_t radius = 0.0f) const { return rndPos(radius, rng); }

  // return a random position at which a circle of that radius fits along an edge
  // avoiding edge close to now
  Vec2 rndEdgePos(fp_t radius, const Vec2 &now) const { return rndEdgePos(radius, now, rng); }

  // similar, but using external rng
  
  // integer [0,n)
  int rndInt(int n, RNG &rng) const;

  // double [0,1)
  fp_t rnd01(RNG &rnd) const;

  // random location for circle that fits
  Vec2 rndPos(fp_t radius, RNG &rng) const;

  // return a random position at which a circle of that radius fits along an edge
  // avoiding edge close to now
  Vec2 rndEdgePos(fp_t radius, const Vec2 &now, RNG &rng) const;

  int getFrameCount() const { return frameCounter; }
  
  fp_t getWidth() const { return width; }
  fp_t getHeight() const { return height; }  

  using UnitMap = std::map<int, Unit>;
  
  const UnitMap &getUnits() const { return id2unit; }

  void addUnit(const Unit &u) { id2unit.insert({ u.unitId, u }); }

  std::pair<int, int> countUnits() const;
  
private:
  
  fp_t width, height;
  bool fogOfWar;
  fp_t qtEps;
  int frameCounter;
  std::vector<Player*> players;
  std::string params;
  std::array<TimeStats, 2> playerStats;
  TimeStats viewStats, actionStats, motionStats, attackStats;
  Timer startTime;
  
  UnitMap id2unit;
  std::array<Quadtree<Unit>, 2> qts;
    
  mutable RNG rng; // to generate random numbers local to simulation

  WorldListener *worldListener;
  
  // computed in each frame
  std::vector<PlayerView> selfViews;
  std::vector<PlayerView> opponentViews; // opponent units player i can see

  // map?

  void computeViews();
  void executeActions();
  
  void executeAttacks();
  void executeMotion();
  void moveUnit(Unit &u);

public:

  // can u be seen by opponent units?
  bool isVisible(const Unit &u);

  // can one unit in units see u?
  static bool canSee(const std::vector<Unit> &units, const Unit &u);

  // can from attck to? (without considering visibility)
  static bool canAttack(const Unit &from, const Unit &to);

  void writeStats() const;
};

