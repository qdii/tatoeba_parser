CC=g++
CFLAGS=-Wall -Wextra -O3 -pedantic -std=c++0x
EXEC=parser
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)
INCLUDES=-I/usr/include/unicode
LIBS=-lboost_regex -licuuc -lboost_program_options -licutu
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)
    
clean:
	rm -rf *.o
