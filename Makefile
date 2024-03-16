PROJECT_NAME = CHIP8

CXX = g++
CXXFLAGS = -Wall -Werror

$(PROJECT_NAME): chip8.cpp main.cpp
	$(CXX) $(CXXFLAGS) -o $(PROJECT_NAME) chip8.cpp main.cpp

clean:
	rm -f $(PROJECT_NAME)