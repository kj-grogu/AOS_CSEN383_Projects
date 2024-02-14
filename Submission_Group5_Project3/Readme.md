# Compilation and Execution

    To compile the project, use the following commands:
    	1. Compiling all files individually and then run:
    		g++ -c utility.c -o utility.o
    		g++ -c concert.c -o concert.o
    		g++ utility.o concert.o -o concert_simulation -lpthread

    	2. Compiling all files together:
    		g++ utility.h utility.c concert.c

    To run the simulation with different numbers of customers per ticket, use one of the following commands:
    	1. commands to use if all files compiled individually:
    		-> For 5 customers:
    		./concert_simulation 5
    		-> For 10 customers:
    		./concert_simulation 10
    		-> For 15 customers:
    		./concert_simulation 15

    	2. commands to use if all files compiled together:
    		-> For 5 customers:
    		./a.out 5
    		-> For 10 customers:
    		./a.out 10
    		-> For 15 customers:
    		./a.out 15
