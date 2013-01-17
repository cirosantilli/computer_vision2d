CC=g++
LIBS=-lglut -lGLU -lGL -lIrrlicht
INCLUDE_DIRS=-I/usr/include/GL -I/usr/include/irrlicht

IN_EXT=.cpp
IN_DIR=./
AUX_DIR=_aux/
AUX_EXT=.o
OUT_DIR=_out/
OUT_EXT=

INS=$(wildcard $(IN_DIR)*$(IN_EXT))
INS_NODIR:=$(notdir $(INS))
AUXS_NODIR:=$(patsubst %$(IN_EXT),%$(AUX_EXT),$(INS_NODIR))
AUXS=$(addprefix $(AUX_DIR),$(AUXS_NODIR))
OUT=$(OUT_DIR)main$(OUT_EXT)

.PHONY: all clean mkdir run 

all: mkdir $(OUT)

run: all
	./$(OUT) $(RUN_ARGS)

$(OUT): $(AUXS)
	$(CC) $^ -o "$@" $(LIBS)

$(AUX_DIR)%$(AUX_EXT): $(IN_DIR)%$(IN_EXT)
	$(CC) $(INCLUDE_DIRS) -c "$<" -o "$@"

mkdir:
	mkdir -p "$(AUX_DIR)"
	mkdir -p "$(OUT_DIR)"

clean:
	rm -rf $(AUX_DIR) $(OUT_DIR)
