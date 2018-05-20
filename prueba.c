#include <stdio.h>
#include <stdlib.h>
#include "eliminar.h"
#include "dispersion.h"

#define LOG(string)                                                                     \
  do{                                                                                   \
    if(debug == 1){                                                                     \
      printf("\n\n\t\t[%s:%d:%s] # %s\n\n",__FILE__,__LINE__,__FUNCTION__,string);      \
      fflush(stdout);                                                                   \
    }                                                                                   \
  }while(0)

#define FICHERO_DAT  "alumnos.dat"
#define FICHERO_HASH "alumnos.hash"

#define DNI1 "7389298"
#define DNI2 "7301387"
#define DNI3 "7219752"
#define DNI4 "123456789"
#define DNI5 "11000258"

void probarBusquedas(char*rutaHash);
void probarEliminaciones(char*rutaHash);


int debug = 0;
int imprimirFichero = 1;

int main(){
  int i;

  printf("\n\n************************ GENERAR HASH ********************************\n");
  printf("\n\tEntrada: %s\n\tSalida: %s",FICHERO_DAT,FICHERO_HASH);
  LOG("Generacion fichero HASH");
  i=creaHash(FICHERO_DAT,FICHERO_HASH);
  LOG("Generacion fichero HASH done");
  printf("\n\tRegistros desbordados: %d\n",i);
  if(imprimirFichero == 1){
    printf("\n\nImpresion de fichero Hash:\n");
    leeHash(FICHERO_HASH);
  }

  printf("\n\n************************ BUSQUEDAS DE PRUEBA *************************\n");
  probarBusquedas(FICHERO_HASH);

  printf("\n\n************************ PROBAR ELIMINACIONES ************************\n");
  probarEliminaciones(FICHERO_HASH);

  printf("\n\n************************ BUSQUEDAS DE COMPROBACION *******************\n");
  probarBusquedas(FICHERO_HASH);

  if(imprimirFichero == 1){
    printf("\n\n************************ IMPRESION DE COMPROBACION *******************\n");
    leeHash(FICHERO_HASH);
  }


  printf("\n\n");

  return 0;
}


void probarBusquedas(char*rutaHash){
  FILE *f;
  tipoAlumno reg;
  int i;

  LOG("Abriendo fichero HASH");
  f=fopen(FICHERO_HASH,"rb");
  LOG("Fichero HASH abierto");

  LOG("Comenzando busqueda 1");
  printf("\n\n\tBuscando %s -- EN BLOQUE ORIGINAL --  HAY REGISTROS DESBORDADOS",DNI1);
  i = buscaReg(f,&reg,DNI1);
  if(i > 0){
  	printf("\n\tRegistro encontrado en el CUBO %d",i);
  	printf("\n\t\t %s %s %s %s %s\n",reg.dni,reg.nombre,reg.ape1,reg.ape2,reg.provincia);
  }else{
  	printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI1,i);
  }

  LOG("Comenzando busqueda 2");
  printf("\n\n\tBuscando %s -- EN BLOQUE ORIGINAL -- NO HAY REGISTROS DESBORDADOS",DNI2);
  i = buscaReg(f,&reg,DNI2);
  if(i > 0){
    printf("\n\tRegistro encontrado en el CUBO %d",i);
    printf("\n\t\t %s %s %s %s %s\n",reg.dni,reg.nombre,reg.ape1,reg.ape2,reg.provincia);
  }else{
    printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI2,i);
  }

  LOG("Comenzando busqueda 3");
  printf("\n\n\tBuscando %s -- EN DESBORDE",DNI3);
  i = buscaReg(f,&reg,DNI3);
  if(i > 0){
  	printf("\n\tRegistro encontrado en el CUBO %d",i);
  	printf("\n\t\t %s %s %s %s %s\n",reg.dni,reg.nombre,reg.ape1,reg.ape2,reg.provincia);
  }else{
  	printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI3,i);
  }

  LOG("Comenzando busqueda 4");
  printf("\n\n\tBuscando %s -- NO ESTA",DNI4);
  i = buscaReg(f,&reg,DNI4);
  if(i > 0){
  	printf("\n\tRegistro encontrado en el CUBO %d",i);
  	printf("\n\t\t %s %s %s %s %s\n",reg.dni,reg.nombre,reg.ape1,reg.ape2,reg.provincia);
  }else{
  	printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI4,i);
  }

  LOG("Comenzando busqueda 5");
  printf("\n\n\tBuscando %s -- EN LA ULTIMA POSICION DEL DESBORDE TRAS LAS OTRAS 3 ELIMINACIONES",DNI5);
  i = buscaReg(f,&reg,DNI5);
  if(i > 0){
    printf("\n\tRegistro encontrado en el CUBO %d",i);
    printf("\n\t\t %s %s %s %s %s\n",reg.dni,reg.nombre,reg.ape1,reg.ape2,reg.provincia);
  }else{
    printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI5,i);
  }

  LOG("Cerrando fichero HASH");
  fclose(f);
  LOG("Fichero HASH cerrado");
}

void probarEliminaciones(char*rutaHash){
  int i;

  LOG("Comenzando eliminacion 1");
  printf("\n\n\tBuscando %s -- EN BLOQUE ORIGINAL -- HAY REGISTROS DESBORDADOS",DNI1);
  i = eliminarReg(FICHERO_HASH,DNI1);
  if(i != -1){
    printf("\n\tRegistro eliminado con exito # estaba en el CUBO %d\n",i);
  }else{
    printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI1,i);
  }

  LOG("Comenzando eliminacion 2");
  printf("\n\n\tBuscando %s -- EN BLOQUE ORIGINAL -- NO HAY REGISTROS DESBORDADOS",DNI2);
  i = eliminarReg(FICHERO_HASH,DNI2);
  if(i != -1){
    printf("\n\tRegistro eliminado con exito # estaba en el CUBO %d\n",i);
  }else{
    printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI2,i);
  }

  LOG("Comenzando eliminacion 3");
  printf("\n\n\tBuscando %s -- EN DESBORDE",DNI3);
  i =eliminarReg(FICHERO_HASH,DNI3);
  if(i != -1){
    printf("\n\tRegistro eliminado con exito # estaba en el CUBO %d\n",i);
  }else{
    printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI3,i);
  }

  LOG("Comenzando eliminacion 4");
  printf("\n\n\tBuscando %s -- NO ESTA",DNI4);
  i = eliminarReg(FICHERO_HASH,DNI4);
  if(i != -1){
    printf("\n\tRegistro eliminado con exito # estaba en el CUBO %d\n",i);
  }else{
    printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI4,i);
  }

  LOG("Comenzando eliminacion 5");
  printf("\n\n\tBuscando %s -- EN LA ULTIMA POSICION DEL DESBORDE TRAS LAS OTRAS 3 ELIMINACIONES",DNI5);
  i = eliminarReg(FICHERO_HASH,DNI5);
  if(i != -1){
    printf("\n\tRegistro eliminado con exito # estaba en el CUBO %d\n",i);
  }else{
    printf("\n\tRegistro con DNI: %s no encontrado; codigo de retorno %d\n",DNI5,i);
  }

  
}