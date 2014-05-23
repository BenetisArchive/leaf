leaf: leaf.c
	gcc -o leaf leaf.c -I . -pthread
debug_leaf: leaf.c
	gcc -g -o leaf leaf.c -I . -pthread
