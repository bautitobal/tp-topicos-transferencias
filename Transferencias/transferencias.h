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
#define ARCH_SALDO_TRIMESTRE "transferencia_totales_trimestres.txt"
#define ARCH_SALDO_ANIO "transferencia_totales_anio.csv"
#define ARCH_SALDO_PAISES_OPERACION "transferencias_totales_paises_operacion.csv"
#define ARCH_SALDO_CONTINENTE_PORCENTAJE "transferencias_totales_continente_porcentaje.dat"

#define TAM_LINEA 300
#define TAM_VEC 5411
#define TAM_COD 3
#define TAM_DESC_PAIS 51
#define TAM_CONTINENTE 16
#define TAM_TRIMESTRE 4
#define DEBITO 'D'
#define CREDITO 'C'
#define CANT_PAISES 198
#define CANT_CONTINENTE 7
#define ANIO_ACTUAL 2026
#define ANIO_MENOR 2015
#define SIN_VARIACION "NA"
#define MENOR_A_MAYOR 1
#define MAYOR_A_MENOR 2
#define AFIRMATIVO 'S'
#define NEGATIVO 'N'

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
    char continente[TAM_CONTINENTE];
    float credito_porcentaje;
    float debito_porcentaje;
}tPorcentajeContinente;
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
void PonerEnCeroVecFloat(float* vec);
void AcumularSaldoTrimestre(tTransferencia* infoT, float* saldoTrimestre,bool* tieneOperacion);
void GenerarArch_SaldoTrimestre(Vector* infoT);
void GenerarArch_SaldoAnio(Vector* infoT);
void GenerarArch_SaldoPais_SegunOperacion(Vector* infoT);
void GenerarArch_SaldoContinentePorcentaje(Vector* infoT);
void CalcularTotalDeOperaciones(Vector* infoT, float* totalDebito, float* totalCredito);
void LeerArchivo_PorcentajeContinente();

int ValidarRango_Int(int LimI, int LimS);
int Cmp_TCod(void* a, void* b);
int cmp_CreditoPais(void* a, void* b);
int CmpCredito_Continente(void* a, void* b);
int Cmp_TDesc(void* a,void* b);
int Cmp_TAnio(void* a, void* b);
int Cmp_SPDebito(void* a, void* b);
int Cmp_SPCredito(void* a, void* b);
int Cmp_Porc_Continente(void* a, void* b);
char ValidarChar(char a, char b);

#endif
