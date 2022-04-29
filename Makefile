CC = clang++
CXXFLAGS= -O2 -g3 -Wall -Wextra -std=c++17
DEPS = Hashtable.h

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

gerp: main.o Hashtable.o
	$(CC) -o gerp main.o Hashtable.o $(CXXFLAGS)

clean:
	rm -f *.o core* *~gerp
