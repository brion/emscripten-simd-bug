all : test.html test-simd.html

test.html : test.c Makefile
	emcc -o test.html -std=c99 -O3 -g4 test.c

test-simd.html : test.c Makefile
	emcc -o test-simd.html -std=c99 -O3 -g4 -s SIMD=1 test.c

clean :
	rm -f test.html test.js test.wasm
	rm -f test-simd.html test-simd.js test-simd.wasm