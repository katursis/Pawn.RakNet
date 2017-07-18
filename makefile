OUTFILE = "pawnraknet.so"
 
COMPILE_FLAGS=-m32 -c -O3 -w -idirafter "lib"
LINK_FLAGS=-m32 -shared -O3 -static-libstdc++

all:
	gcc $(COMPILE_FLAGS) lib/SDK/amx/*.h
	g++ $(COMPILE_FLAGS) -std=c++11 lib/SDK/*.cpp	
	g++ $(COMPILE_FLAGS) -std=c++11 lib/RakNet/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++11 src/*.cpp
	g++ $(LINK_FLAGS) -o $(OUTFILE) *.o
	rm *.o
	strip -s $(OUTFILE) 