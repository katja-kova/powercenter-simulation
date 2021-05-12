# Makefile for the powercenter component
#
CPP 	= g++
CC		= gcc
OBJ		= *.o
SOURCE	= *.cpp
HEADER	= *.h
BIN		= perftest
RM		= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) -static-libgcc -pthread

$(OBJ): $(SOURCE)
	$(CPP) -c $? -std=c++11
