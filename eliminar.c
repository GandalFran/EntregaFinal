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
#define ESTA_VACIA(string) (string[0] == '\0')


int extraerRegDesborde(char*dni,int hash,int tipoBusqueda,FILE*f,tipoAlumno*a);
int extraeUltimoRegDesborde(FILE*f,tipoAlumno*a);
void recolocarCubo(tipoCubo*c, int aSustituir);
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

	encontrado = 0;
	for(i=0; i<c.numRegAsignados-1 && i<C-1; i++){
		if(SON_IGUALES(c.reg[i].dni,dni)){
			encontrado = 1;
			break;
		}
	}


	if(encontrado){//Registro encontrado en cubo de su hash

		if(ESTA_DESBORDADO(c)){
			extraerRegDesborde(NULL,HASH(dni),CON_HASH,f,&a);
			c.reg[i] = a;
			decrementarNumRegsDesborde(f);
		}else
			recolocarCubo(&c,i);

		c.numRegAsignados -= 1;
		fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
		fwrite(&c,TAM_CUBO,1,f);
	

		fclose(f);
		return 2;

	}else if(ESTA_DESBORDADO(c)){//Registro no econtrado pero esta en el desborde

		extraerRegDesborde(dni,0,CON_DNI,f,&a);

		fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
		fread(&c,TAM_CUBO,1,f);
		c.numRegAsignados -= 1;
		fwrite(&c,1,TAM_CUBO,f);

		//decrementarNumRegsDesborde(f);

		fclose(f);
		return 1;

	}else{//No se va a poder encontrar
		fclose(f);
		return -1;
	}

	
}


int extraerRegDesborde(char*dni,int hash,int tipoBusqueda,FILE*f,tipoAlumno*a){
	int i,j,encontrado;	
	tipoCubo c;
	tipoAlumno regSustituir;

	fseek(f,INICIO_DESBORDE,SEEK_SET);

	encontrado = 0;
	for(i=0; i<CUBOSDESBORDE && !encontrado; i++){
		fread(&c,TAM_CUBO,1,f);
		for(j=0; j<C && j<c.numRegAsignados; j++){
			if(tipoBusqueda == CON_HASH){
				if(hash == HASH(c.reg[j].dni)){
					encontrado = 1;
					break;
				}
			}else{
				if(SON_IGUALES(c.reg[j].dni,dni)){
					encontrado = 1;
					break;
				}
			}
		}
	}	

	*a = c.reg[j];

	extraeUltimoRegDesborde(f,&regSustituir);

	if(SON_IGUALES(c.reg[j].dni,regSustituir.dni) || ESTA_VACIA(regSustituir.dni)){
		recolocarCubo(&c,j);
	}else{
		c.reg[j] = regSustituir;
	}

	fseek(f,DESPLAZA_A_CUBO(CUBOS+i-1),SEEK_SET);
	fwrite(&c,TAM_CUBO,1,f);

	return 0;
}

int extraeUltimoRegDesborde(FILE*f,tipoAlumno*a){
	int i;
	tipoCubo c, anterior;

	fseek(f,INICIO_DESBORDE,SEEK_SET);
	
	for(i=0; i<CUBOSDESBORDE; i++){
		anterior = c;
		fread(&c,TAM_CUBO,1,f);
		if(c.numRegAsignados == 0)
			break;
	}

	if(i > 0)
		c = anterior;


	*a = c.reg[c.numRegAsignados-1];

	recolocarCubo(&c,c.numRegAsignados-1);

	fseek(f,-2*TAM_CUBO,SEEK_CUR);
	fwrite(&c,TAM_CUBO,1,f);

	return 0;
}

void recolocarCubo(tipoCubo*c, int aSustituir){
	int j;

	for(j=c->numRegAsignados-1; j>=0; j--){
		if(!ESTA_VACIA(c->reg[j].dni)){
			c->reg[aSustituir] = c->reg[j];
			break;
		}
	}

	memset(&(c->reg[j]),0,TAM_ALUMNO);
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
