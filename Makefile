
all: prog

prog: src/*.cpp src/*.h
	cd src && g++ main.cpp -o ../prog
