# Makefile for miner_tb.cpp 
CPP = /usr/bin/g++ 
INCLUDE = -I ${MGC_HOME}/shared/include

TARGET = miner
OBJECTS = miner.o miner_tb.o 

${TARGET}: ${OBJECTS} 

${OBJECTS}: miner.cpp miner_tb.cpp miner.h Makefile 

%.o : %.cpp 
	${CPP} -c ${INCLUDE} $< 

${TARGET}: ${OBJECTS} 
	${CPP} ${OBJECTS} ${LINKARGS} -o $@ 

clean: 
	rm -rf *.o ${TARGET}