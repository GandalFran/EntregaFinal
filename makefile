#entregaEda
prueba: prueba.c eliminar.o dispersion.o
	gcc -g eliminar.o dispersion.o prueba.c -o prueba
eliminar.o: eliminar.c eliminar.h
	gcc -g -c eliminar.c
dispersion.o: dispersion.c dispersion.h
	gcc -g -c dispersion.c