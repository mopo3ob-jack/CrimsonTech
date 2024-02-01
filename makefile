CFLAGS = -O2 -std=c++23
LDFLAGS = -lGL -lglfw

SOURCE = $(shell find ./src/ -name "*.cpp")
OBJECT = $(patsubst ./src/%,./lib/%,$(patsubst %.cpp,%.o,$(SOURCE)))

default: main.cpp crimson ./glad/lib/glad.o
	g++ $(CFLAGS) $^ $(LDFLAGS) -o main

crimson: $(OBJECT)
	ld -r $^ -o crimson

./glad/lib/glad.o:
	gcc -c -O2 ./glad/src/glad.c -o glad/lib/glad.o

./lib/%.o: ./src/%.cpp
	g++ -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(shell find ./lib/ -name "*.o")