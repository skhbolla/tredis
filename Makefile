all: include/* src/*
	gcc -I include src/*.c -o tredis
clean:
	rm ./tredis
