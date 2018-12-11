# makefile for csim
#
#  make -j4 [MODE=dbg|opt] refreshes executable, compiles using CCOPTS_<MODE>
#  make clean              removes all object files, dependencies, and executable

# inspired by
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# (gcc section)

PROG := csim

SRCDIR := src
OBJBASE := obj
DEPBASE := dep

# dbg or opt
MODE = dbg

# uncomment for debug mode
CCOPTS_dbg := -Wall -Wextra -Wconversion -O -g -std=c++17

# uncomment for release mode
CCOPTS_opt := -Wall -Wextra -O3 -DNDEBUG -std=c++17

ifeq ("$(MODE)", "dbg")

  OBJDIR := $(OBJBASE)/dbg
  DEPDIR := $(DEPBASE)/dbg
  CCOPTS := $(CCOPTS_dbg)

else ifeq ("$(MODE)", "opt")

  OBJDIR := $(OBJBASE)/opt
  DEPDIR := $(DEPBASE)/opt
  CCOPTS := $(CCOPTS_opt)

else

$(error "unknown MODE $(MODE)")

endif


SOURCES = \
  csim.cpp \
  Gfx.cpp \
  P_IndCtrl.cpp \
  Player.cpp \
  PlayerView.cpp \
  Quadtree.cpp \
  Unit.cpp \
  UnitTypes.cpp \
  World.cpp \
  W_Plain.cpp

LIBS = -lglut -lGL -lboost_program_options

SRCS = $(addprefix $(SRCDIR)/, $(SOURCES))
OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))

$(shell mkdir -p $(DEPBASE)/dbg $(DEPBASE)/opt $(OBJBASE)/dbg $(OBJBASE)/opt  >/dev/null)

# ensure that executable is up to date when switching MODE
$(shell rm -f $(PROG) >/dev/null)

CC := g++
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
COMPILE.cpp = $(CC) $(DEPFLAGS) $(CCOPTS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

# link object files
$(PROG) : $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

# how to create .o files from .c files
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPDIR)/%.d makefile
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

# remove object files, dependencies, and executable
clean:  
	rm -f $(OBJBASE)/dbg/* $(OBJBASE)/opt/* $(DEPBASE)/dbg/* $(DEPBASE)/opt/* $(PROG)

# include dependencies
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES))))

#
