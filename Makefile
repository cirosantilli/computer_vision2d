CC := g++
LIBS := -lglut -lGLU -lGL -lIrrlicht
#INCLUDE_DIRS := -I/usr/include/GL -I/usr/include/irrlicht

IN_EXT := .cpp
OBJ_EXT := .o
OUT_EXT := .out

INS := $(wildcard *$(IN_EXT))
OBJS := $(INS:$(IN_EXT)=$(OBJ_EXT))
OUT := main$(OUT_EXT)

.PHONY: all clean run

all: $(OUT)

run: all
	./$(OUT) $(RUN_ARGS)

$(OUT): $(OBJS)
	$(CC) $^ -o '$@' $(LIBS)

%$(OBJ_EXT): %$(IN_EXT)
	$(CC) -c '$<' -o '$@'

clean:
	rm -rf *$(OBJ_EXT) *$(OUT_EXT)
