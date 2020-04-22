# Makefile

all: main clean

main: main.o matrix.o sector.o
	g++ -o main main.o matrix.o sector.o

main.o: main.cpp
	g++ -c main.cpp

matrix.o: src/Matrix.cpp src/Matrix.h
	g++ -c src/Matrix.cpp -o matrix.o

sector.o: src/Sector.cpp src/Sector.h
	g++ -c src/Sector.cpp -o sector.o

clean:
	rm -f *.o
