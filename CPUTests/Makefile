CC = g++
FLAGS = -std=c++17 -DDEBUG -Werror
VPATH = ../
OBJECTS = CPUTests.o CPUTestsMain.o Memory.o CPU.o

cputest: $(OBJECTS)
	$(CC) $(OBJECTS) -o cputest

CPUTestsMain.o: CPUTestsMain.cpp catch.hpp
	$(CC) $(FLAGS) -c CPUTestsMain.cpp

CPUTests.o: CPUTests.cpp Types.h DummyPortInterface.hpp catch.hpp Memory.hpp CPU.hpp 
	$(CC) $(FLAGS) -I.. -c CPUTests.cpp
	
Memory.o: Memory.cpp Memory.hpp Types.h
	$(CC) $(FLAGS) -I.. -c ../Memory.cpp

CPU.o: CPU.cpp Memory.hpp Types.h Instructions.h CPU.hpp PortInterface.hpp
	$(CC) $(FLAGS) -I.. -c ../CPU.cpp

clean:
	rm cputest *.o
