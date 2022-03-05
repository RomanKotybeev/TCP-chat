CXX = g++
CXXFLAGS = -Wall -g
OBJMODULES = selector.o server.o client.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

chat: chat.cpp $(OBJMODULES)
	$(CXX) $(CXXFLAGS) $^ -o $@
	rm -f *.o

run: main
	./main

clean:
	rm -f *.o main
