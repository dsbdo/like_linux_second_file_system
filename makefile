all: main.out
main.out: main.o FileProcess.o CommandParse.o UserMoudle.o InitMoudle.o
	g++ main.o	FileProcess.o InitMoudle.o CommandParse.o UserMoudle.o -o main.out -g 

	
main.o: main.cpp 
	g++ -c main.cpp -g
	
FileProcess.o: ./FileProcess/FileProcess.cpp 
	g++ -c ./FileProcess/FileProcess.cpp -g

InitMoudle.o: ./InitMoudle/InitMoudle.cpp
	g++ -c  ./InitMoudle/InitMoudle.cpp -g

CommandParse.o: ./CommandParse/CommandParse.cpp 
	g++ -c ./CommandParse/CommandParse.cpp -g

UserMoudle.o: ./UserMoudle/UserMoudle.cpp 
	g++ -c ./UserMoudle/UserMoudle.cpp -g
clean:
	rm -rf *.o
	rm -rf SystemDisk
	rm -rf *.bin