PROJECT_NAME = CHIP8

CXX = g++ --std=c++17
CXXFLAGS = -Wall -Werror -Idependencies/sdl2/include
LDFLAGS = -Ldependencies/sdl2/lib
INCDIR = -lSDL2main -lSDL2

all: $(PROJECT_NAME)

$(PROJECT_NAME): main.cpp chip8.cpp display.cpp
	$(CXX) $(CXXFLAGS) ${LDFLAGS} -o $(PROJECT_NAME) main.cpp chip8.cpp display.cpp -lmingw32 $(INCDIR)

.PHONY: clean
clean:
	rm -f $(PROJECT_NAME)