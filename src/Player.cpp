#include "Player.h"

#include "World.h"

PlayerView &Player::self()
{
  return world->getSelfView(playerId);
}
  
const PlayerView &Player::opponent() const
{
  return world->getOpponentView(playerId);
}
