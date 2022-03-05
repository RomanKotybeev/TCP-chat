CXX = g++
CXXFLAGS = -Wall -g
OBJMODULES = selector.o server.o client.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

chat: chat.cpp $(OBJMODULES)
	$(CXX) $(CXXFLAGS) $^ -o $@
	rm -f *.o

run: chat
	./chat

clean:
	rm -f *.o chat
