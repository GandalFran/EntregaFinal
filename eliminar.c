#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eliminar.h"

/*Tipos de busqueda para la funcion estraerRegDesborde(...)*/
#define CON_HASH 0
#define CON_DNI 1

#define TAM_CUBO (sizeof(tipoCubo))
#define TAM_ALUMNO (sizeof(tipoAlumno))

/*-----------------------------------------------------------------------------------------------------*/

#define HASH(elemento) ((atoi(elemento)) % CUBOS)

#define DESPLAZA_A_CUBO(numcubo) ( TAM_CUBO * ( numcubo ) )
#define INICIO_DESBORDE 		 ( DESPLAZA_A_CUBO( CUBOS ) )
#define FIN_DESBORDE 			 ( DESPLAZA_A_CUBO( CUBOS + CUBOSDESBORDE - 1 ) )

#define ESTA_DESBORDADO(cubo) (cubo.numRegAsignados > C)
#define SON_IGUALES(dni1,dni2) (!strcmp(dni1,dni2))
#define ESTA_VACIA(string) (string[0] == '\0')

/*-----------------------------------------------------------------------------------------------------*/

/**
*	Descripcion: 
*		Busca el registro indicado en el desborde; lo sustituye por el ultimo registro de desborde
*		y lo devuelve en a.
*	@param dni: dni a buscar si el tipo de busqueda es CON_DNI
*	@param hash: hash a buscar si el tipo de busqueda es CON_HASH
*	@param tipoBusqueda: CON_DNI o CON_HASH
*	@param f: fichero en el que buscar
*	@param a: registro donde meter el restulado de la busqueda
*
*	@return: 0 si todo es correcto y -1 si se da alguna condicion de error
*/
int extraerRegDesborde(char*dni,int hash,int tipoBusqueda,FILE*f,tipoAlumno*a);

/**
*	Descripcion: 
*		Busca ultimo registro del desborde, lo borra y lo devuelve en a.
*	@param f: fichero en el que buscar
*	@param a: registro donde meter el restulado de la busqueda
*
*	@return: 0 si todo es correcto y -1 si se da alguna condicion de error
*/
int extraeUltimoRegDesborde(FILE*f,tipoAlumno*a);

/**
*	Descripcion: 
*		Busca el ultimo registro del cubo, lo borra y lo coloca en 
*		la posicion aSustituir del cubo.
*	@param c: cubo sobre el que trabajar
*	@param aSustituir: posicion a sustituir
*/
void recolocarCubo(tipoCubo*c, int aSustituir);

/**
*	Descripcion: 
*		Decrementa en uno el atributo numRegAsignados en cada
*		cubo del desborde.
*	@param f: fichero sobre el que trabajar
*/
void decrementarNumRegsDesborde(FILE*f);

/*-----------------------------------------------------------------------------------------------------*/

int eliminarReg(char*fichero, char *dni){
	FILE*f;
	tipoCubo c;
	tipoAlumno a;
	int i,encontrado,retorno;	

	if(NULL == (f = fopen(fichero,"r+b"))){
		return -2;
	}

	fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
	fread(&c,TAM_CUBO,1,f);

	encontrado = 0;
	for(i=0; i<=c.numRegAsignados-1 && i<C-1; i++){
		if(SON_IGUALES(c.reg[i].dni,dni)){
			encontrado = 1;
			break;
		}
	}


	if(encontrado){//Registro encontrado en cubo de su hash

		if(ESTA_DESBORDADO(c)){
			if(0 != extraerRegDesborde(NULL,HASH(dni),CON_HASH,f,&c.reg[i]))
				return -2;
			decrementarNumRegsDesborde(f);
		}else
			recolocarCubo(&c,i);

		c.numRegAsignados -= 1;
		fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
		fwrite(&c,TAM_CUBO,1,f);

		fclose(f);
		return 2;

	}else if(ESTA_DESBORDADO(c)){//Registro no econtrado pero puede estar en el desborde

		if(0  != extraerRegDesborde(dni,0,CON_DNI,f,&a))
			return -1;
		decrementarNumRegsDesborde(f);

		fseek(f,DESPLAZA_A_CUBO(HASH(dni)),SEEK_SET);
		fread(&c,TAM_CUBO,1,f);
		c.numRegAsignados -= 1;
		fseek(f,-TAM_CUBO,SEEK_CUR);
		fwrite(&c,TAM_CUBO,1,f);

		fclose(f);
		return 1;

	}else{//No esta en el fichero

		fclose(f);
		return -1;

	}
}

/*-----------------------------------------------------------------------------------------------------*/

int extraerRegDesborde(char*dni,int hash,int tipoBusqueda,FILE*f,tipoAlumno*a){
	tipoCubo c;
	tipoAlumno regSustituir;
	int i,j,encontrado,retorno;	

	fseek(f,INICIO_DESBORDE,SEEK_SET);

	encontrado = 0;
	for(i=0; i<CUBOSDESBORDE && !encontrado; i++){

		fread(&c,TAM_CUBO,1,f);

		for(j=0; j<C && j<=c.numRegAsignados; j++){
			if(tipoBusqueda == CON_HASH){
				if(hash == HASH(c.reg[j].dni)){
					encontrado = 1;
					break;
				}
			}else if(tipoBusqueda == CON_DNI){
				if(SON_IGUALES(c.reg[j].dni,dni)){
					encontrado = 1;
					break;
				}
			}else{
				return -1;
			}
		}
	}	

	if(!encontrado)
		return -1;

	*a = c.reg[j];

	retorno = extraeUltimoRegDesborde(f,&regSustituir);

	//Si estamos tratando el ultimo registro del desborde o no quedan registros en el desborde
	if(SON_IGUALES(c.reg[j].dni,regSustituir.dni) || retorno != 0){
		recolocarCubo(&c,j);
	}else{
		c.reg[j] = regSustituir;
	}

	fseek(f,DESPLAZA_A_CUBO(CUBOS+i-1),SEEK_SET);
	fwrite(&c,TAM_CUBO,1,f);

	return 0;
}

/*-----------------------------------------------------------------------------------------------------*/

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

	//No se ha encontrado ninguno
	if(i>=CUBOSDESBORDE)
		return -1;
	
	//Se mete en c el cubo a tratar a excepcion de que no haya nada en anterior
	if(i > 0)	
		c = anterior;

	*a = c.reg[c.numRegAsignados-1];

	recolocarCubo(&c,c.numRegAsignados-1);

	fseek(f,-2*TAM_CUBO,SEEK_CUR);
	fwrite(&c,TAM_CUBO,1,f);

	return 0;
}

/*-----------------------------------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------------------------------*/

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