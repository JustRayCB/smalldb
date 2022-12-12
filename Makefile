CXXFLAGS+=-std=c++17  -fsanitize=thread -g -Wall -Wextra -Wpedantic -D_GNU_SOURCE -Werror=all -O2
#-fsanitize=undefined,leak,address
CXX=g++
LDLIBS+=-lpthread
SOURCES = $(wildcard *.cpp */*.cpp)
HEADERS = $(wildcard *.hpp */*.hpp)

OBJ=utils.o db.o student.o query.o parsing.o


main: smalldb sdbsh
sdbsh: sdbsh.cpp
	$(CXX) -o sdbsh sdbsh.cpp $(CXXFLAGS) $(LDLIBS)


smalldb: smalldb.cpp ${OBJ}
	$(CXX) -o smalldb smalldb.cpp ${OBJ} $(CXXFLAGS) $(LDLIBS)



%.o: %.cpp %.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

clean:
	-rm smalldb
	-rm sdbsh
	-rm *.hpp.gch
	-rm *.o
