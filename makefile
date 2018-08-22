main: main.o FileProcess.o InitMoudle.o CommandParse.o UserMoudle.o
	g++ main.o	FileProcess.o InitMoudle.o CommandParse.o UserMoudle.o -o main.out -g 
main.o: main.cpp ./FileProcess/FileProcess.cpp  ./InitMoudle/InitMoudle.cpp ./UserMoudle/UserMoudle.cpp
	g++ -c main.cpp -g
	
FileProcess.o: ./FileProcess/FileProcess.cpp 
	g++ -c ./FileProcess/FileProcess.cpp -g

InitMoudle.o: ./FileProcess/FileProcess.cpp ./InitMoudle/InitMoudle.cpp
	g++ -c  ./InitMoudle/InitMoudle.cpp -g

CommandParse.o: ./CommandParse/CommandParse.cpp ./UserMoudle/UserMoudle.cpp ./FileProcess/FileProcess.cpp
	g++ -c ./CommandParse/CommandParse.cpp -g

UserMoudle.o: ./UserMoudle/UserMoudle.cpp ./FileProcess/FileProcess.cpp
	g++ -c ./UserMoudle/UserMoudle.cpp -g
