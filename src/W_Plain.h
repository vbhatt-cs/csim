#pragma once

#include "World.h"

class W_Plain : public World
{
public:

  W_Plain() { }

  void setup(fp_t width, fp_t height, bool fogOfWar, int seed,
             Player *p0, Player *p1,
             fp_t qtEps,
             const std::string &params) override;

  bool gameFinished() const override;
  
  int score0(bool timeout) const override;
};
