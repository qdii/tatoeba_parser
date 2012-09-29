CC=g++
CFLAGS=-Wall -Wextra -O3 -pedantic -std=c++0x
EXEC=parser
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)
LIBS=-lboost_regex -lboost_program_options
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)
    
clean:
	rm -rf *.o
