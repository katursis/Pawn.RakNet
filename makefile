OUTFILE = "pawnraknet.so"

COMPILE_FLAGS=-m32 -c -O3 -w -Ilib
LINK_FLAGS=-m32 -shared -O3 -static-libgcc -static-libstdc++

all:
	g++ $(COMPILE_FLAGS) -std=c++17 lib/RakNet/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++17 src/*.cc
	g++ $(LINK_FLAGS) -o $(OUTFILE) *.o
	rm *.o
