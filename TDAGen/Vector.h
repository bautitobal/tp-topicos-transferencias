#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../codigoError.h"

#define INCREMENTO_VEC 5
typedef struct{

    void* vec;
    size_t ce;
    size_t tam;
    size_t tipo;
}Vector;
typedef int (*Cmp)(void* a,void* b);

Vector* vectorCrear(size_t tam,size_t tipo);
int vectorInsertarFinal(Vector* v,void* elem);
void MostrarVec(Vector* v);
int vectorInsertarEnPos(Vector* v,void* elem,int pos);
int vectorOrdInsertar(Vector*,void*,Cmp cmp);
void vectorOrdenar(Vector*,Cmp cmp,int opcion);
void* Menor(Vector*,void*,Cmp cmp);
void* Mayor(Vector* v, void* actual,Cmp cmp);
void Swap(void*,void*,size_t);
int vectorDesordBuscar(Vector*,void*,Cmp cmp);
int vectorOrdBuscar(Vector*,void*,Cmp cmp);
bool vectorBorrarPorPos(Vector* v, int pos);
bool vectorDesordBorrar(Vector*v, void* elem,Cmp cmp);
bool vectorOrdBorrar(Vector*v, void* elem,Cmp cmp);
bool vectorBorrarTodaAparicion(Vector* v, void* elem,Cmp cmp);
void vectorDestruir(Vector* v);
void* vectorGetElementoPorPos(Vector* v, int pos);
size_t vectorGetCantElem(Vector*);

#endif
