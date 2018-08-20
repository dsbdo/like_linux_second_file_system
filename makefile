main: main.o FileProcess.o InitMoudle.o
	g++ main.o	FileProcess.o InitMoudle.o -o main.out
main.o: main.cpp ./FileProcess/FileProcess.cpp  ./InitMoudle/InitMoudle.cpp
	g++ -c main.cpp 
	
FileProcess.o: ./FileProcess/FileProcess.cpp 
	g++ -c ./FileProcess/FileProcess.cpp 

InitMoudle.o: ./FileProcess/FileProcess.cpp ./InitMoudle/InitMoudle.cpp
	g++ -c  ./InitMoudle/InitMoudle.cpp 
