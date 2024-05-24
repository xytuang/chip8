CXX = g++
CXXFLAGS = -std=c++0x -Wall -g
LDFLAGS = -rdynamic

SRCS = main.cpp chip8.cpp
OBJS = ${SRCS:.cpp=.o}
HEADERS = 

MAIN = main

all: ${MAIN}

${MAIN}: ${OBJS}
	${CXX} ${LDFLAGS} ${OBJS} -o ${MAIN}

.cpp.o:
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	${RM} ${PROGS} ${MAIN} ${OBJS} *.o *~. 
