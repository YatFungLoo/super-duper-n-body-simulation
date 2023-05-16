### Makefile for C++ ###

MF=Makefile

EXE=2BodySim
INC=
SRC=test_basicTwoBodySim.cpp

# CXX=g++
CXX=gcc
CXXFLAGS=
LFLAGS=
# LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system
LDFLAGS=-lm -lstdc++

.SUFFIXES:
.SUFFIXES: .cpp .o

OBJ=$(SRC:.cpp=.o) 

.cpp.o:	
	$(CXX) $(CXXFLAGS) -c $<

all:	$(EXE)

$(OBJ):	$(INC)

$(EXE):	$(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS) $(LDFLAGS)

$(OBJ):	$(MF)

clean:
	rm -f $(OBJ) $(EXE) core
