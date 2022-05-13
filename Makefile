all: ttfpatcher

ttfpatcher: ttfpatcher.c
	gcc ttfpatcher.c -o ttfpatcher
