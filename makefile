all: compile run sanitizer

compile:
	g++ -o main main.cpp -g -O2

run:
	valgrind ./main > ans.out

sanitizer:
	g++ -o main main.cpp -g -O2 -fsanitize=address
	./main > ans.out

