.PHONY: all clean run

COMPILER = g++
COMPILER_FLAG = -std=c++20 -fmodules-ts

run: main
	./main

main: build/gc.o build/main.o
	${COMPILER} $^ -o main

build/%.o: src/%.cpp 
	mkdir -p build
	${COMPILER} -c  ${COMPILER_FLAG} $< -o $@

clean:
	@-rm main
	@-rm -r build
	@-rm -r gcm.cache
