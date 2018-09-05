all: cache

cache: main.cpp Cachee.cpp Cache.h
	g++  main.cpp Cachee.cpp -o cache -std=c++11 -g

run: all
	./cache traces/trace1.txt mytrace1_output.txt
	./cache traces/trace2.txt mytrace2_output.txt
	./cache traces/trace3.txt mytrace3_output.txt
	
checkmem: all
	valgrind ./cache traces/trace1.txt mytrace1_output.txt
	
clean:
	rm -fv cache *.o 
