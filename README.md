# csim
A simple, fast, and extensible RTS game combat simulator written in C++

(c) 2018 Michael Buro ; Licensed under GPLv3

---

Currently supported:

- two AI players (RED vs BLUE) playing a single RTS combat game
- games are played on empty (width x height) maps
- unit types: tanks, marines (basic stats copied from BroodWar) (see UnitTypes*)
- sample world W_Plain supports random start state (tanks,marines in mirrored
  locations)
- linear any-angle motion to (x,y) at max-speed, stop when running into border
- no unit-unit collisions
- attacking units within attack range (Quadtree used to speed this up)
- removing units with hp <= 0
- select policy for each player (string parameter)

---

Speed:

- scripts/demo.qt.test
- 2 x (2000 marines + 2000 tanks)
- 16384 x 8192 pixel map (marine/tank stats normalized to SC:BW dimensions)
- all units moving towards random location on map border
- AI players are running target selection code (but don't actually fire)
- 2 AI players + simulation: 100 fps on single core (i7-3630QM CPU @ 2.40GHz)
- 2 AI players + simulation + gfx: 30 fps

---

To come:

- [soon] store replays / replay them
- [soon] serialize states / actions (to interact with Python ML code)
- [soon] better AI players (train NNs)
- [soon] fog of war (implemented, but not tested yet)
- [later] Windows/MacOS build system
- [maybe] collisions + obstacles + layers
- [maybe] better gfx

---

Files:

src/

- csim.cpp   contains main, handles options and single game
- Global.h   common global includes, definitions, and classes
- World.*    world base class; represents game (units, players, ...)
- WorldListener.h  world client interface
- Player.*   AI player base class
- PlayerView.* world view for players (detaches players from world)
- W_*        sample world
- P_*        sample player
- Unit*      unit types
- Quadtree.h for faster attack / visibility test in sparse worlds
             (currently only used in P_*, can be switched on/off there)
 -Gfx.*      displays world, is a WorldListener

doc/
- documentation

scripts/
- some demo scripts

---

dependencies:

- g++ 7+
- boost-devel (program_options)
- glut/GL

---

compile:

 - make -j4 [MODE=opt|dbg]
 - creates ./csim
  
---

run:

- ./csim -g 1
- ./csim --help
- ./scripts/demo.small.gfx
- ./scripts/demo.large.gfx
- ./scripts/demo.small
- ./scripts/demo.large  
  
---

extend:

- add your own worlds (W_*) and players (P_*)
- register them in csim.cpp (newPlayer/newWorld)
- select them for playing via command line options
