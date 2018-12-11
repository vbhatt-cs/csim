/*

  Combat Simulator ("csim")

  (c) 2019 Michael Buro

  Licensed under GPLv3

  see doc/README.txt for instructions
  
 */

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <boost/program_options.hpp>
#include <GL/glut.h>

#include "World.h"
#include "Player.h"
#include "Gfx.h"
#include "Unit.h"
#include "UnitTypes.h"
#include "P_IndCtrl.h"
#include "W_Plain.h"

using namespace std;
namespace po = boost::program_options;

// global => glut functions can access it

World *world = nullptr;
Player *pRed, *pBlue;
int maxSteps = 0;
int delay = 0; // frame delay in ms
Gfx *gfx = nullptr;


static void cleanup()
{
  delete pRed;
  delete pBlue;
  delete gfx;
  delete world;
}


static void execStep()
{
  cout << "." << flush;

  if (world->getFrameCount() < maxSteps && world->executeFrame()) {
    
    if ((world->getFrameCount() % 100) == 0) {
      cout << endl;
      world->writeStats();
      if (gfx) {
        cout << "display millis: " << gfx->displayStats.avgMillis() << endl;
      }
    }
    return;
  }

  // done
  
  cout << endl;
  world->writeStats();
  if (gfx) {
    cout << "display millis: " << gfx->displayStats.avgMillis() << endl;
  }

  int rScore = world->score0(world->getFrameCount() >= maxSteps);
  
  cout << "### game over after " << world->getFrameCount()
       << " step(s) ; RED score: "
       << rScore;

  if (rScore > 0) {
    cout << " ; RED-wins";
  } else if (rScore == 0) {
    cout << " ; tie";
  } else {
    cout << " ; BLUE-wins";
  }

  if (world->getFrameCount() >= maxSteps) {
    cout << " ; timeout ";
  }
  cout << endl;

  cleanup();
  exit(0);
}

void timerFunction(int)
{
  execStep();
  glutTimerFunc((unsigned int)delay, timerFunction, 0);
}

static World *newWorld(const string &worldType)
{
  if (worldType == "Plain") {

    return new W_Plain();

  } else {

    ERR("unknown world type '" << worldType << "'");
    
  }

  return nullptr;
}


static Player *newPlayer(const string &playerType, World *world,
                         int index, const string &playerName,
                         fp_t qtEps,
                         const string &params, int seed)
{
  if (playerType == "IndCtrl") {

    return new P_IndCtrl(world, index, playerName, qtEps, params, seed);
    
  } else {

    ERR("unknown player type '" << playerType << "'");

  }
  
  return nullptr;
}


void onDisplay()
{
  // done in WorldListener callback
}


int main(int argc, char *argv[])
{
#if 0
  // test quadtree
  quadtreeTest();
  exit(0);
#endif
  
  // Declare the supported options
  po::options_description desc("Options");

  desc.add_options()
    ("help", "produce help message")
    ("world", po::value<string>()->default_value("Plain"), "set world type")
    ("wpar", po::value<string>()->default_value("100 100"), "set world parameters")
    ("width,w", po::value<fp_t>()->default_value(800), "set world width")
    ("height,h", po::value<fp_t>()->default_value(800), "set world height")
    ("fow", po::bool_switch()->default_value(false), "swtich fog of war on")
    ("qteps", po::value<fp_t>()->default_value(800), "set quadtree split epsilon (0: no qt, 800 good for large W_Plain)")
    ("delay,d", po::value<int>()->default_value(50), "set frame delay (ms)")
    ("seed,s", po::value<int>()->default_value(0), "set rng seed (0:time)")
    ("rplayer", po::value<string>()->default_value("IndCtrl"), "set red player")
    ("bplayer", po::value<string>()->default_value("IndCtrl"), "set blue player")
    ("rpar,r", po::value<string>()->default_value("attack_closest"), "set red parameters (attack_non|closest|weakest|most_dangerous)")
    ("bpar,b", po::value<string>()->default_value("attack_closest"), "set blue parameters (attack_non|closest|weakest|most_dangerous)")
    ("steps", po::value<int>()->default_value(-1), "maximum steps (-1: infinite)")
    ("graphics,g", po::value<double>()->default_value(0.0), "graphics scaling factor (0: no gfx)");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  string worldType = vm["world"].as<string>();
  string wPar = vm["wpar"].as<string>();
  
  fp_t width = vm["width"].as<fp_t>();
  fp_t height = vm["height"].as<fp_t>();

  bool fow = vm["fow"].as<bool>();
  fp_t qtEps = vm["qteps"].as<fp_t>();
  
  delay = vm["delay"].as<int>(); // frame delay in ms
  int seed = vm["seed"].as<int>();   // for rng, 0: use time

  string redPlayer = vm["rplayer"].as<string>();
  string bluePlayer = vm["bplayer"].as<string>();
  
  string rPar = vm["rpar"].as<string>();
  string bPar = vm["bpar"].as<string>();
  
  int steps = vm["steps"].as<int>();
  double gfxScale = vm["graphics"].as<double>();
  
  if (seed == 0) {
    // rng seed dependent on wallclock time
    auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::system_clock::now().time_since_epoch()).count();

    seed = (int)millis;
  }

  cout << "world:   " << world      << endl;
  cout << "wpar:    " << wPar       << endl;
  cout << "width:   " << width      << endl;
  cout << "height:  " << height     << endl;
  cout << "fow:     " << fow        << endl;
  cout << "qteps:   " << qtEps      << endl;
  cout << "delay:   " << delay      << endl;
  cout << "seed:    " << seed       << endl;
  cout << "rplayer: " << redPlayer  << endl;
  cout << "bplayer: " << bluePlayer << endl;
  cout << "rpar:    " << rPar       << endl;
  cout << "bpar:    " << bPar       << endl;
  cout << "steps:   " << steps      << endl;

  if (gfxScale <= 0) {
    cout << "gfx: "      << "none" << endl;
  } else {
    cout << "gfx: "      << "scale " << gfxScale << endl;
  }

  // set up world

  world = newWorld(worldType);

  // set up players

  Player *pRed  = newPlayer(redPlayer,  world, 0, "RED_"  + redPlayer,  qtEps, rPar, seed);
  Player *pBlue = newPlayer(bluePlayer, world, 1, "BLUE_" + bluePlayer, qtEps, bPar, seed);

  // wire up components

  world->setup(width, height, fow, seed, pRed, pBlue, qtEps, wPar);

  // run game
  
  maxSteps = 100'000;

  if (steps != -1) {
    maxSteps = min(steps, maxSteps);
  }
  
  if (gfxScale != 0) {
    
    glutInit(&argc, argv);
    glutInitWindowSize((int)ceil(width * gfxScale), (int)ceil(height * gfxScale));
    glutInitDisplayMode(GLUT_DOUBLE);
    glutCreateWindow("CSIM - Combat Simulator");
    
    glutDisplayFunc(onDisplay);
    glutTimerFunc(1, timerFunction, 0);

    gfx = new Gfx(world);
    world->setListener(gfx);

    glutMainLoop();
    return 0;
  }

  // no gfx

  for (;;) {
    execStep();
  }

  return 0;
}
