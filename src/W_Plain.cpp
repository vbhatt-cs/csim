#include "W_Plain.h"

#include <sstream>
#include "UnitTypes.h"

using namespace std;

void W_Plain::setup(fp_t width, fp_t height, bool fogOfWar, int seed,
                    Player *p0, Player *p1,
                    fp_t qtEps,
                    const std::string &params)
{
  World::setup(width, height, fogOfWar, seed, p0, p1, qtEps, params);

  istringstream is(params);

  int nMarines, nTanks;
  
  is >> nMarines >> nTanks;

  if (!is || nMarines < 0 || nTanks < 0 || nMarines + nTanks <= 0) {
    ERR("W_Plain: missing or illegal parameters (nMarines nTanks) '" << params << "'");
  }

  // populate with units

  // get unit sizes - a bit awkward

  Unit m = makeMarine(RED);
  fp_t mr = m.radius;

  m = makeTank(RED);
  fp_t tr = m.radius;

  // create marines
  
  for (int i=0; i < nMarines; ++i) {

    Unit u = makeMarine(RED, rndPos(mr));
    addUnit(u);

    // mirrored
    
    Unit v = makeMarine(BLUE, mirror(u.pos));
    addUnit(v);
  }

  // create tanks
  
  for (int i=0; i < nTanks; ++i) {

    Unit u = makeTank(RED, rndPos(tr));
    addUnit(u);

    // mirrored
    
    Unit v = makeTank(BLUE, mirror(u.pos));
    addUnit(v);
  }
}


bool W_Plain::gameFinished() const
{
  auto uc = countUnits();
  return uc.first == 0 || uc.second == 0;
}
  

int W_Plain::score0(bool /*timeout*/) const
{
  auto uc = countUnits();

  if (uc.first == 0 && uc.second == 0) {
    return 0; // draw
  }

  if (uc.first > 0 && uc.second == 0) {
    return 1; // win
  }

  if (uc.first == 0 && uc.second > 0) {
    return -1; // loss
  }

  return 0; // draw
}
