CC=clang++
CPPFLAGS=-std=c++17
DEPS=
OBJ = shunty.cpp
INC=-I${HOME}/Code/range-v3/include/

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS) $(INC)

shunty: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS) $(INC)
