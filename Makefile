leaf: leaf.c testF.c
	gcc -o leaf leaf.c testF.c -I . -pthread
debug_leaf: leaf.c testF.c
	gcc -g -o leaf leaf.c testF.c -I . -pthread
