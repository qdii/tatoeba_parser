CXX=g++
CXXFLAGS=-Wall -Wextra -O3 -pedantic -std=c++0x
EXEC=parser
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)
LIBS=-lboost_regex -lboost_program_options
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIBS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES)
    
clean:
	rm -rf *.o
