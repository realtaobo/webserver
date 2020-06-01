main: 
	g++ -g -Wall  main.cpp base/*.cpp Log/*.cpp Reactor/*.cpp -o main -lpthread -std=c++11
clean:
	-rm main