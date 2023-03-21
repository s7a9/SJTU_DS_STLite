all: compile run sanitizer

compile:
	g++ -o main main.cpp -g

run:
	valgrind ./main

sanitizer:
	g++ -o main main.cpp -g -fsanitize=address
	./main

