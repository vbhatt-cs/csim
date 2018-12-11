#pragma once

#include "Player.h"

class P_IndCtrl : public Player
{
public:

  enum Policy { ATTACK_NONE=0, ATTACK_CLOSEST, ATTACK_WEAKEST, ATTACK_MOST_DANGEROUS };

  P_IndCtrl(World *world_, int playerId_, const std::string &name_, fp_t qtEps_,
            const std::string &pol_, int seed_)
  {
    setup(world_, playerId_, name_, qtEps_, pol_, seed_);
    polEnum = policyFromString(pol_);
  }
  
  // returns policy name
  static std::string policyToString(Policy pol);

  static Policy policyFromString(const std::string &ps);

  void onFrame(int /*frameCount*/) override;

  void onGameEnd() override;

private:

  Policy polEnum;
};
