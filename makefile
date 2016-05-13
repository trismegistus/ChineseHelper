CC = clang++
CFLAGS = --std=c++11 -g `pkg-config --libs --cflags gtk+-3.0`

all:
	$(CC) $(CFLAGS) Converter.cpp CharacterDB.cpp Vowels.cpp UI.cpp main.cpp -o main

