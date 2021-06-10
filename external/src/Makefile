# Makefile for the powercenter component
#
CPP 	= g++
CC		= gcc
OBJ		= *.o
SOURCE	= *.cpp 
PROTOSRC = *.cc
HEADER	= *.h
BIN		= powercenter
RM		= rm -f
DIR_PROTO	= /app/protos
DIR_INC		= /app/.local/include
DIR_LIB		= /app/.local/lib
DIR_SRC		= /app/cpp/src
PROTOFILE	= history.proto
PROTO_LOC	= $(DIR_PROTO)/$(PROTOFILE)
PROTOC		= protoc

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(SOURCE) $(HEADER)

$(BIN): $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) -static-libgcc -L $(DIR_LIB) -lprotobuf -std=c++11

$(OBJ): $(SOURCE) $(PROTOSRC)
	$(CPP) -c $? -std=c++11 -I $(DIR_INC)

$(PROTOSRC): $(PROTO_LOC)
	$(PROTOC) -I $(DIR_PROTO) --grpc_out=$(DIR_SRC) --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $(PROTO_LOC)
	$(PROTOC) -I $(DIR_PROTO) --cpp_out=$(DIR_SRC) $(PROTO_LOC)
