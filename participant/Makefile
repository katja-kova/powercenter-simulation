# Makefile for the participant component
#
CPP 	= g++
CC		= gcc
OBJ		= *.o
SOURCE	= *.cpp
HEADER	= *.h
BIN		= participant
RM		= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(SOURCE) $(HEADER)

$(BIN): $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) -static-libgcc

$(OBJ): $(SOURCE)
	$(CPP) -c $? -std=c++11
