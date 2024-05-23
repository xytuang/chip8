CXX = g++
CXXFLAGS = -std=c++0x -Wall -g
LDFLAGS = -rdynamic

SRCS = chip8.cpp
OBJS = ${SRCS:.cpp=.o}
HEADERS = 

MAIN = chip8

all: ${MAIN}

${MAIN}: ${OBJS}
	${CXX} ${LDFLAGS} ${OBJS} -o ${MAIN}

.cpp.o:
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	${RM} ${PROGS} ${MAIN} ${OBJS} *.o *~. 
