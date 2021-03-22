#!/bin/sh


CC=gcc
LIBS=-lm -lcrypt
FLAGS=-O3 -fPIC -shared $(shell pkg-config --cflags python2) -I.

SRC_FILES=$(wildcard *.c)
OBJ_FILES=$(addsuffix .o,$(basename $(SRC_FILES))) libruby-static.a


all: pyrb.so

clean:
	rm -f pyrb.so *.o


%.o: %.c
	$(CC) $(FLAGS) -c $^

libruby-static.a:

pyrb.so: $(OBJ_FILES)
	$(CC) $(FLAGS) $(LIBS) $^ -o pyrb.so
