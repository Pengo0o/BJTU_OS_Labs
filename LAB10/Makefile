TARGET := bank
SRC := ${wildcard ./src/*.c}
C_EXTENSION := -lreadline -lncurses
CC := gcc

all:
	${CC} -o ${TARGET} ${SRC} ${C_EXTENSION}
	./${TARGET}
