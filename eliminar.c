#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eliminar.h"

#define MARCA(str, ...)										\
	fprintf(stderr,"\n[%s:%d] ",__FUNCTION__,__LINE__);		\
	fprintf(stderr, str, ##__VA_ARGS__);					\
	fprintf(stderr,"\n");

#define HASH(elemento) ((atoi(elemento)) % CUBOS)

#define TAM_CUBO (sizeof(tipoCubo))
#define TAM_ALUMNO (sizeof(tipoAlumno))

#define INICIO_DESBORDE (CUBOS*TAM_CUBO)
#define FIN_DESBORDE ((CUBOS+CUBOSDESBORDE-1)*TAM_CUBO)
#define DESPLAZA_HASH(elemento) (HASH(elemento)*TAM_CUBO)

#define ESTA_DESBORDADO(cubo) (cubo.numRegAsignados > C)
#define SON_IGUALES(dni1,dni2) (!strcmp(dni1,dni2))


tipoAlumno buscarYBorrarRegistroEnDesborde(int hash,FILE*f);

int eliminarReg(char*fichero, char *dni){
	int i,j,tope;	
	FILE*f;
	tipoCubo c;

	if(NULL == (f = fopen(fichero,"r+b"))){
		return -2;
	}

	fseek(f,DESPLAZA_HASH(dni),SEEK_SET);
	fread(&c,TAM_CUBO,1,f);

	tope = (c.numRegAsignados < C)? (c.numRegAsignados) : (C);
	for(i=0; i<tope; i++){
		if(SON_IGUALES(c.reg[i].dni,dni)){
			if(ESTA_DESBORDADO(c)){
				c.reg[i] = buscarYBorrarRegistroEnDesborde(HASH(dni),f);
			}else{
				c.reg[i] = c.reg[c.numRegAsignados-1];
				memset(&c.reg[c.numRegAsignados-1],0,sizeof(tipoAlumno));
			}

			c.numRegAsignados -= 1;

			fseek(f,DESPLAZA_HASH(dni),SEEK_SET);
			fwrite(&c,1,TAM_CUBO,f);

			fclose(f);
			return HASH(dni);
		}
	}

	if(!ESTA_DESBORDADO(c)){
		fclose(f);
		return -1;
	}else{
		c.numRegAsignados -= 1;
		fseek(f,-TAM_CUBO,SEEK_CUR);
		fwrite(&c,1,TAM_CUBO,f);
	}

	fseek(f,INICIO_DESBORDE,SEEK_SET);

	for(i=0; i<CUBOSDESBORDE; i++){
		fread(&c,TAM_CUBO,1,f);
		tope = (c.numRegAsignados < C)? (c.numRegAsignados) : (C);
		for(j=0; j<tope; j++){
			if(SON_IGUALES(c.reg[j].dni,dni)){
				c.reg[j] = buscarYBorrarRegistroEnDesborde(-1,f);
				fseek(f,INICIO_DESBORDE+(TAM_CUBO*i),SEEK_SET);
				fwrite(&c,1,TAM_CUBO,f);
				fclose(f);
				return CUBOS+i;
			}
		}
	}

}

/*
si el hash es <0 entonces devuelve el ultimo registro del fichero
*/
tipoAlumno buscarYBorrarRegistroEnDesborde(int hash,FILE*f){
	tipoCubo c;
	tipoAlumno a;
	int i, j, encontrado;

	int bloque;

	fseek(f,FIN_DESBORDE,SEEK_SET);

	bloque = CUBOS+CUBOSDESBORDE;
	encontrado = 0;
	while(!encontrado && bloque>=CUBOS){
		fread(&c,TAM_CUBO,1,f);
		fseek(f,-2*TAM_CUBO,SEEK_CUR);

		for(i=c.numRegAsignados-1; i>=0; i--){
			if(hash > 0){
				if(HASH(c.reg[i].dni) == hash){
					encontrado = 1;
					break;
				}
			}else{
				if(!SON_IGUALES(c.reg[i].dni,"\0")){
					encontrado = 1;
					break;
				}
			}
		}
		bloque--;
	}
	fseek(f,2*TAM_CUBO,SEEK_CUR); //para avanzar lo que retrocedimos

	a = c.reg[i];

	for(j=c.numRegAsignados-1; j>=0; j--){
		if(!SON_IGUALES(c.reg[j].dni,"\0")){
			c.reg[i] = c.reg[j];
			break;
		}
	}

	memset(&c.reg[j],0,sizeof(tipoAlumno));
	c.numRegAsignados -= 1;

	fseek(f,-TAM_CUBO,SEEK_CUR);
	fwrite(&c,1,TAM_CUBO,f);

	return a;
}