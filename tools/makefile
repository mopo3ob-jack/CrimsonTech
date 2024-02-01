CFLAGS = -O2 -std=c++23 -fsanitize=leak
LDFLAGS = -lGL -lglfw -lassimp

default: ./bin/bsp

./bin/%: ./src/%.cpp
	g++ $(CFLAGS) $< $(LDFLAGS)  -o $@