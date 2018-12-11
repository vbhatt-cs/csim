# csim
A simple combat simulator

(c) 2018 Michael Buro
Licensed under GPLv3

This is a simple, fast, and extensible RTS game combat simulator

Currently supported:

- two AI players playing a single RTS combat game (add more players in newPlayer())
- select policy for each player (string parameter)
- unit types: tanks, marines (stats copied from BroodWar) (see UnitTypes*)
- empty width x height map
- sample world W_Plain supports random start state (tanks,marines in mirrored
  locations)
- no unit-unit collisions
- linear motion to (x,y) at max-speed, stop when running into border
- attacking units within attack range
- removing units with hp <= 0

To come:

- [soon] store replays / replay them
- [soon] serialize states / actions (to interact with Python ML code)
- [soon] better AI players (train NNs)
- [soon] fog of war (implemented, but not tested yet)
- [later] Windows/MacOS build system
- [maybe] collisions + obstacles + layers
- [maybe] better gfx

============================================================================

Files:

src/

  csim.cpp   contains main, handles options and single game
  Global.h   common global includes, definitions, and classes
  World.*    world base class; represents game (units, players, ...)
  WorldListener.h  world client interface
  Player.*   AI player base class
  PlayerView.* world view for players (detaches players from world)
  W_*        sample world
  P_*        sample player
  Unit*      unit types
  Quadtree.h for faster attack / visibility test in sparse worlds
             (currently only used in P_*, can be switched on/off there)
  Gfx.*      displays world, is a WorldListener

doc/
  documentation

scripts/
  some demo scripts

============================================================================

dependencies:

  g++ 7+
  boost-devel (program_options)
  glut/GL

============================================================================

compile:

  make -j4 [MODE=opt|dbg]

  creates ./csim
  
============================================================================

run:

  ./csim --help
  ./scripts/demo.small.gfx
  ./scripts/demo.large.gfx
  ./scripts/demo.small
  ./scripts/demo.large  
  
============================================================================

extend:

- add your own worlds (W_*) and players (P_*)
- register them in csim.cpp (newPlayer/newWorld)
- select them for playing via command line options
