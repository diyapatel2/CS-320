#P: 
#all: $(P)

#$(P): $(P).o Project1.o
#	g++ $(CFLAGS) $(P).o -o predictors 
#$(P).o: $(P).cpp
#	g++ -c $(P).cpp
#Project1.o: Project1.cpp
#	g++ -c Project1.cpp 
#clean: 
#	rm -rf *.o predictors 
#run: all
#	./$(P)


#all: driver

#driver: main.cpp Predictor.cpp Predictor.h 
#	g++ main.cpp Predictor.cpp -o predictors -std=c++11

#clean:
#	rm -f predictors

all: driver

driver: driver.cpp Predict.cpp Predict.h 
	g++ driver.cpp Predict.cpp -o predictors -std=c++11

clean:
	rm -f predictors
