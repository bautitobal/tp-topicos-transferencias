#ifndef TRANSFERENCIAS_H
#define TRANSFERENCIAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../TDAGen/Vector.h"



#define ARCH_TRANSFERENCIA "..\\transferencias_personales_clean.csv"
#define ARCH_PAISES "..\\paises_continentes.csv"
#define ARCH_TRANSTOTAL_PAISES "transferencias_totales_paises.csv"
#define ARCH_SALDO_CONTINENTE "transferencias_totales_continentes.csv"
#define TAM_LINEA 300
#define TAM_VEC 5411
#define TAM_COD 3
#define TAM_DESC_PAIS 51
#define TAM_CONTINENTE 16
#define DEBITO 'D'
#define CREDITO 'C'
#define CANT_PAISES 198

typedef struct
{
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    char continente[TAM_CONTINENTE];
}tPais;


typedef struct
{
    int anio;
    int trimestre;
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    char operacion;
    float monto;
}tTransferencia;

typedef struct
{
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    int cantRegistros;
    float credito;
    float debito;
} tSaldoPais;

typedef struct
{
    char continente[TAM_CONTINENTE];
    int cantRegistros;
    float credito;
    float debito;
} tSaldoContinente;

void trozarCampoVariable_Pais(tPais *info,char* linea);
void trozarCampoVariable_Transferencia(tTransferencia* infoT, char* linea);
FILE* AbrirArch(const char* nombre,const char* modo);
void CargarInfoTransferencia(Vector* infoT, FILE* pf);
void CerrarArch(FILE* p);
void GenerarArch_SaldoPorPais(Vector* infoT);
void GenerarArch_SaldoPorContinente(Vector* infoT);
void AcumularSaldosSegunContinente(Vector* infoC,tPais* infoPais,tTransferencia* vecT);
void InicializarVectorSaldoContinente(Vector* infoC);
int Cmp_T(void* a, void* b);
int cmp_CreditoPais(void* a, void* b);
int CmpCredito_Continente(void* a, void* b);

#endif
