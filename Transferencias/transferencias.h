#ifndef TRANSFERENCIAS_H
#define TRANSFERENCIAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../TDAGen/Vector.h"



#define ARCH_TRANSFERENCIA "../transferencias_personales_clean.csv"
#define ARCH_PAISES "../paises_continentes.csv"
#define ARCH_TRANSTOTAL_PAISES "transferencias_totales_paises.csv"
#define ARCH_SALDO_CONTINENTE "transferencias_totales_continentes.csv"
#define ARCH_SALDO_TRIMESTRE "transferencia_totales_trimestres.txt"
#define ARCH_SALDO_ANIO "transferencia_totales_anio.csv"
#define ARCH_SALDO_PAISES_OPERACION "transferencias_totales_paises_operacion.csv"
#define ARCH_SALDO_CONTINENTE_PORCENTAJE "participacion_continentes.dat"
#define ARCH_BASE_LARGA "transferencias_base_larga.csv"
#define ARCH_PROMEDIO_PAIS_TRIMESTRE "transferencias_promedio_pais_trimestre.csv"
#define ARCH_PROMEDIO_CONTINENTE_ANI_OP "transferencias_promedio_continente_anio_op.csv"

#define TAM_LINEA 300
#define TAM_VEC 5411
#define TAM_COD 3
#define TAM_DESC_PAIS 51
#define TAM_CONTINENTE 16
#define TAM_TRIMESTRE 4
#define TAM_VEC_MIN 50
#define TAM_RESPUESTA 3

#define DEBITO 'D'
#define CREDITO 'C'
#define AFIRMATIVO 'S'
#define NEGATIVO 'N'

#define CANT_PAISES 198
#define CANT_CONTINENTE 7
#define ANIO_ACTUAL 2026
#define ANIO_MENOR 2015
#define MENOR_A_MAYOR 1
#define MAYOR_A_MENOR 2

#define SIN_VARIACION "NA"
#define COD_MUNDO "W0"
#define COD_INDETERMINADO "_X"
#define NO_CALCULABLE "N/C"

int compararSinMayusculas(const char* cadena1, const char* cadena2);

typedef struct
{
    /* código y descripción del país junto con su continente. */
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    char continente[TAM_CONTINENTE];
}tPais;


typedef struct
{
    /* registro original de una transferencia. */
    int anio;
    int trimestre;
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    char operacion;
    float monto;
}tTransferencia;

typedef struct
{
    /* porcentajes de crédito y débito correspondientes a un continente. */
    char continente[TAM_CONTINENTE];
    float credito_porcentaje;
    float debito_porcentaje;
}tPorcentajeContinente;
typedef struct
{
    /* totales acumulados para un país. */
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    int cantRegistros;
    float credito;
    float debito;
} tSaldoPais;

typedef struct
{
    /* totales acumulados para un continente. */
    char continente[TAM_CONTINENTE];
    int cantRegistros;
    float credito;
    float debito;
} tSaldoContinente;

typedef struct 
{
    /* promedios agrupados por país y tipo de operación. */
    char desc_pais[TAM_DESC_PAIS];
    char operacion;
    float sumaMontos;
    int cant_trimestre;
    float promedio_debito;
    float promedio_credito;
}tPromedio_Pais_Trimestre;

typedef struct
{
    /* registro de la base larga, que agrega el continente al movimiento. */
    int anio;
    int trimestre;
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    char continente[TAM_CONTINENTE];
    char operacion;
    float monto;
}tBase_Larga;

typedef struct{
    char cod_pais[TAM_COD];
    char desc_pais[TAM_DESC_PAIS];
    float total_credito;
    float total_debito;
    float total_neto;
} tTransferenciaPorMonto;

/* Los prototipos siguen el mismo orden que las funciones en transferencias.c */

/* ---- funciones compartidas (se usan desde más de un punto) ----
   compararSinMayusculas está declarada más arriba, junto a los #define */
FILE* AbrirArch(const char* nombre,const char* modo);
int ValidarRango_Int(int LimI, int LimS);
char ValidarChar(char a, char b);
bool trozarCampoVariable_Pais(tPais *info,char* linea);
int InicializarVectorSaldoContinente(Vector* continente);
void AcumularSaldosSegunContinente(Vector* continente, tPais* pais, tTransferencia* transferencia);
int Cmp_TDesc(void* a,void* b);

/* ---- carga inicial de las transferencias ---- */
bool trozarCampoVariable_Transferencia(tTransferencia* transferencia, char* linea);
int CargarInfoTransferencia(Vector* transferencia, FILE* pf);

/* ---- opción 1: totales por país ---- */
int GenerarArch_SaldoPorPais(Vector* transferencia);
int Cmp_TCod(void* a, void* b);
int cmp_CreditoPais(void* a, void* b);

/* ---- opción 2: totales por continente ---- */
int GenerarArch_SaldoPorContinente(Vector* transferencia);
int CmpCredito_Continente(void* a, void* b);

/* ---- opción 3: saldo trimestral por país ---- */
int GenerarArch_SaldoTrimestre(Vector* transferencia);
void PonerEnCeroVecFloat(float* vec);
void AcumularSaldoTrimestre(tTransferencia* transferencia, float* saldoTrimestre,bool* tieneOperacion);

/* ---- opción 4: totales anuales y variación interanual ---- */
int GenerarArch_SaldoAnio(Vector* transferencia);
int Cmp_TAnio(void* a, void* b);

/* ---- opción 5: ranking de países por crédito o débito ---- */
int GenerarArch_SaldoPais_SegunOperacion(Vector* transferencia);
int Cmp_SPCredito(void* a, void* b);
int Cmp_SPDebito(void* a, void* b);

/* ---- opción 6: participación porcentual por continente ---- */
int GenerarArch_SaldoContinentePorcentaje(Vector* transferencia);
void CalcularTotalDeOperaciones(Vector* transferencia, float* totalDebito, float* totalCredito);
int Cmp_Porc_Continente(void* a, void* b);
int LeerArchivo_PorcentajeContinente();

/* ---- opción 7: generación de la base larga ---- */
int GenerarArch_BaseLarga(Vector* transferencia);
int Cmp_Transferencia_VariosCampos(void* a, void* b);
void EncontrarContinente_SegunPais(char* continente, tTransferencia* auxT, FILE* pPais);

/* ---- carga de la base larga (opciones 8 y 9) ---- */
bool trozarCampoVariable_BaseLarga(tBase_Larga* baseLarga,char* linea);
int CargarInfo_BaseLarga(Vector* baseLarga, FILE* pf);

/* ---- opción 8: promedio trimestral por país ---- */
int GenerarArch_Promedio_Pais_Trimestre(Vector* baseLarga);
int Cmp_BaseLarga_Pais_Operacion(void* a, void* b);
int Cmp_PPT_DescPais(void* a, void* b);
int Cmp_PPT_PromedioCredito(void* a, void* b);

/* ---- opción 9: promedio por continente, operación y año ---- */
int GenerarArch_Promedio_Anio_Continente_Op(Vector* baseLarga);
int Cmp_Prom_ACO(void* a, void* b);

/* ---- opción 10: ranking de países según el saldo neto ---- */
int RankPorSaldoNeto(Vector* infoT);
int Cmp_TPM_Neto(void*, void*);

/* ---- opción 11: evolución anual de las operaciones de un país ---- */
int EvolucionAnualPais(Vector* infoT);

#endif
