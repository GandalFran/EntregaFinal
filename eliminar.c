#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eliminar.h"

#define MARCA fprintf(stderr,"\n[%s:%d]",__FUNCTION__,__LINE__);

#define HASH(elemento) ((elemento) % CUBOS)

#define TAM_CUBO (sizeof(tipoCubo))
#define TAM_ALUMNO (sizeof(tipoAlumno))

#define INICIO_DESBORDE (CUBOS*TAM_CUBO)
#define DESPLAZA_HASH(elemento) (HASH(elemento)*TAM_CUBO)

#define ESTA_DESBORDADO(cubo) (cubo.numRegAsignados > C)


int eliminarReg(char*fichero, char *dni){
	int i,j,tope;	
	FILE*f;
	tipoAlumno a;
	tipoCubo c;

	if(NULL == (f = fopen(fichero,"r+b"))){
		return -2;
	}

	fseek(f,DESPLAZA_HASH(atoi(dni)),SEEK_SET);
	fread(&c,TAM_CUBO,1,f);


	tope = (c.numRegAsignados < C)? (c.numRegAsignados) : (C);
	for(i=0; i<tope; i++){
		if(!strcmp(c.reg[i].dni,dni)){
			if(ESTA_DESBORDADO(c)){

			}else{
				c.reg[i] = c.reg[c.numRegAsignados];
				memset(&c.reg[c.numRegAsignados],0,sizeof(tipoAlumno));
				c.numRegAsignados -= 1;

				fseek(f,-TAM_CUBO,SEEK_CUR);
				fwrite(&c,1,TAM_CUBO,f);
			}
			fclose(f);
			return HASH(atoi(dni));
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
	for(j=0; j<CUBOSDESBORDE; j++){
		fread(&c,TAM_CUBO,1,f);
		tope = (c.numRegAsignados < C)? (c.numRegAsignados) : (C);
		for(i=0; i<tope; i++){
			if(!strcmp(c.reg[i].dni,dni)){
				//Encontrado en desborde
				fclose(f);
				return CUBOS+j;
			}
		}
	}

}