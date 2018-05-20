#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eliminar.h"

#define CON_HASH 0
#define CON_DNI 1

#define MARCA(str, ...)										\
	fprintf(stderr,"\n[%s:%d] ",__FUNCTION__,__LINE__);		\
	fprintf(stderr, str, ##__VA_ARGS__);					\
	fprintf(stderr,"\n");

#define HASH(elemento) ((atoi(elemento)) % CUBOS)

#define TAM_CUBO (sizeof(tipoCubo))
#define TAM_ALUMNO (sizeof(tipoAlumno))

#define DESPLAZA_A_CUBO(numcubo) ( TAM_CUBO * ( numcubo ) )
#define INICIO_DESBORDE 		 ( DESPLAZA_A_CUBO( CUBOS ) )
#define FIN_DESBORDE 			 ( DESPLAZA_A_CUBO( CUBOS + CUBOSDESBORDE - 1 ) )
#define DESPLAZA_HASH(elemento)  ( DESPLAZA_A_CUBO( HASH(elemento) ) )

#define ESTA_DESBORDADO(cubo) (cubo.numRegAsignados > C)
#define SON_IGUALES(dni1,dni2) (!strcmp(dni1,dni2))
#define ESTA_VACIA(string) (!strcmp(string,""))


tipoAlumno extraerRegDesborde(char*dni,int hash,int tipoBusqueda,FILE*f);
tipoAlumno extraeUltimoRegDesborde(FILE*f);
void recolocarCubo(tipoCubo c, int aSustituir);
void decrementarNumRegsDesborde(FILE*f);







int leeHash2(FILE*f){
  tipoCubo cubo;
  int j,i=0;
  size_t numLee;

   rewind(f);
   fread(&cubo,sizeof(cubo),1,f);
   while (!feof(f)){
	for (j=0;j<C;j++) {
        if (j==0)    	fprintf(stderr,"Cubo %2d (%2d reg. ASIGNADOS)",i,cubo.numRegAsignados);
        else  	fprintf(stderr,"\t\t\t");
	if (j < cubo.numRegAsignados) 
		    fprintf(stderr,"\t%s %s %s %s %s\n",
	    		cubo.reg[j].dni,
			cubo.reg[j].nombre,
			cubo.reg[j].ape1,
		  	cubo.reg[j].ape2,
  	                cubo.reg[j].provincia);
	else fprintf (stderr,"\n");
        }
       i++;
       fread(&cubo,sizeof(cubo),1,f);
   }
   return i;
}

void printCubo(tipoCubo cubo, int numCubo){
	int j;
for (j=0;j<C;j++) {
        if (j==0)    	fprintf(stderr,"Cubo %2d (%2d reg. ASIGNADOS)",numCubo,cubo.numRegAsignados);
        else  	fprintf(stderr,"\t\t\t");
	if (j < cubo.numRegAsignados) 
		    fprintf(stderr,"\t%s %s %s %s %s\n",
	    		cubo.reg[j].dni,
			cubo.reg[j].nombre,
			cubo.reg[j].ape1,
		  	cubo.reg[j].ape2,
  	                cubo.reg[j].provincia);
	else fprintf (stderr,"\n");
        }
}




int eliminarReg(char*fichero, char *dni){
	int i,j,tope,encontrado, posCubo, posReg;	
	FILE*f;
	tipoCubo c;
	tipoAlumno a;

	if(NULL == (f = fopen(fichero,"r+b"))){
		return -2;
	}

	fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
	fread(&c,TAM_CUBO,1,f);

	i = -1;
	encontrado = 0;
	while( i<c.numRegAsignados-1 && i<C-1 && !encontrado){
		i++;
		if(SON_IGUALES(c.reg[i].dni,dni))
			encontrado = 1;
	}

	if(encontrado){//Registro encontrado en cubo de su hash

		if(ESTA_DESBORDADO(c)){
			c.reg[i] = extraerRegDesborde(NULL,HASH(dni),CON_HASH,f);
			decrementarNumRegsDesborde(f);
		}else
			recolocarCubo(c,i);
		
		c.numRegAsignados -= 1;
		fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
		fwrite(&c,1,TAM_CUBO,f);

		fclose(f);
		return 2;

	}else if(ESTA_DESBORDADO(c)){//Registro no econtrado pero esta en el desborde
	
		a = extraerRegDesborde(dni,0,CON_DNI,f);

		fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
		fread(&c,TAM_CUBO,1,f);
		c.numRegAsignados -= 1;
		fwrite(&c,1,TAM_CUBO,f);

		decrementarNumRegsDesborde(f);

		fclose(f);
		return 1;

	}else{//No se va a poder encontrar
		fclose(f);
		return -1;
	}

}


tipoAlumno extraeUltimoRegDesborde(FILE*f){

	tipoCubo c;
	tipoAlumno a, regSustituir;
	int i, j, encontrado,bloque;

	fseek(f,FIN_DESBORDE,SEEK_SET);

	//Buscamos desde el final 
	bloque = CUBOS+CUBOSDESBORDE-1;
	encontrado = 0;
	while(!encontrado && bloque>=CUBOS){
		bloque--;

		fread(&c,TAM_CUBO,1,f);
		fseek(f,-2*TAM_CUBO,SEEK_CUR);

		i = C;
		while(i>0 && !encontrado){	
			i--;
			if(!ESTA_VACIA(c.reg[i].dni))
				encontrado = 1;
		}
	}

int k;
MARCA(" ");
printCubo(c,bloque);
for(k=0; k<C; k++){
	MARCA("c.reg[%d].dni :: %s",k,c.reg[k].dni);
}
MARCA("DNI que devuelve %s",c.reg[i].dni);

	//Si lo encontramos lo guardamos y recolocamos el cubo
	if(encontrado){
		a = c.reg[i];
		recolocarCubo(c,i);

		fseek(f,TAM_CUBO,SEEK_CUR); 
		fwrite(&c,1,TAM_CUBO,f);
	}else
		memset(&a,0,TAM_ALUMNO);

	return a;
}


tipoAlumno extraerRegDesborde(char*dni,int hash,int tipoBusqueda,FILE*f){
	tipoCubo c;
	tipoAlumno a, regSustituir;
	int i, j, encontrado,cuboActual;

	fseek(f,INICIO_DESBORDE,SEEK_SET);

	//Buscamos desde el final o bien el registro con hash el que tenemos o bien el ultimo
	cuboActual = CUBOS-1;
	encontrado = 0;
	while(!encontrado && cuboActual<CUBOS+CUBOSDESBORDE-1){
		cuboActual++;

		fread(&c,TAM_CUBO,1,f);

		i = -1;
		while(i<c.numRegAsignados-1 && !encontrado){
			i++;
			if(tipoBusqueda == CON_HASH){
				if(HASH(c.reg[i].dni) == hash)
					encontrado = 1;
			}else{
				if(SON_IGUALES(c.reg[i].dni,dni))
					encontrado = 1;
			}
		}
	}
	MARCA("FICHERO ANTES DE BUSCAR EL ULTIMO REGISTRO: ");
	leeHash2(f);
	MARCA("FIN DE FICHERO");

	a = c.reg[i];
	//Buscamos el ultimo registro del desborde para sustituirlo
	regSustituir = extraeUltimoRegDesborde(f);


	MARCA("DNI encontrado: %s",a.dni);
	MARCA("DNI a sustituir: %s",regSustituir.dni);

	if(ESTA_VACIA(regSustituir.dni) || SON_IGUALES(a.dni,regSustituir.dni)){
		//Si no se encuentra o es el registro que vamos a devolver
		recolocarCubo(c,i);
	}else{
		//En caso contrario lo metemos ahi para sustituir
		c.reg[i] = regSustituir;
	}

	fseek(f, DESPLAZA_A_CUBO(cuboActual),SEEK_SET); 
	fwrite(&c,1,TAM_CUBO,f);


	return a;
}

void recolocarCubo(tipoCubo c, int aSustituir){
	int j;

	for(j=c.numRegAsignados-1; j>=0; j--){
		if(!ESTA_VACIA(c.reg[j].dni)){
			c.reg[aSustituir] = c.reg[j];
			break;
		}
	}

	memset(c.reg[j].dni,'\0',sizeof(tipoAlumno));
	memset(&(c.reg[j]),0,sizeof(tipoAlumno));
}

void decrementarNumRegsDesborde(FILE*f){
	int i;
	tipoCubo c;

	fseek(f,INICIO_DESBORDE,SEEK_SET);

	i=0;
	while(i<CUBOSDESBORDE){
		fread(&c,TAM_CUBO,1,f);
		if(c.numRegAsignados>0){
			c.numRegAsignados -= 1;
			fseek(f,-TAM_CUBO,SEEK_CUR);
			fwrite(&c,1,TAM_CUBO,f);
		}
		i++;
	}

}
