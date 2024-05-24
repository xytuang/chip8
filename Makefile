CXX = g++
CXXFLAGS = -I/Library/Frameworks/SDL2.framework/Headers -std=c++0x -Wall -g
LDFLAGS = -F/Library/Frameworks -framework SDL2

SRCS = main.cpp chip8.cpp
OBJS = ${SRCS:.cpp=.o}

MAIN = main

all: ${MAIN}

${MAIN}: ${OBJS}
	${CXX} ${OBJS} -o ${MAIN} ${LDFLAGS}

.cpp.o:
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	${RM} ${MAIN} ${OBJS} *.o *~
