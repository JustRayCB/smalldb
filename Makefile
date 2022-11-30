CXXFLAGS+=-std=c++17 -fsanitize=undefined,leak,address -g -Wall -Wextra -Wpedantic -D_GNU_SOURCE -Werror=all
CXX=g++
LDLIBS+=-lpthread
SOURCES = $(wildcard *.cpp */*.cpp)
HEADERS = $(wildcard *.hpp */*.hpp)

OBJ=utils.o

.PHONY: main
main: smalldb

#smalldb: smalldb.cpp ${OBJ}
	#$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)

#sdbsh: sdbsh.cpp #${OBJ}
	#$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)



smalldb: smalldb.cpp ${OBJ}
	$(CXX) -o smalldb.cpp ${OBJ} $(CXXFLAGS) $(LDLIBS)


%.o: %.cpp %.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

.PHONY: clean
clean:
	-rm *.o
