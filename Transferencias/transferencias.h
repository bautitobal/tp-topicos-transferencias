#ifndef TRANSFERENCIAS_H
#define TRANSFERENCIAS_H

#define TAM_CODIGO 3 // codigo de paises -> 'AR', 'BRA', etc.
#define TAM_PAIS 50 // nombre de los paises -> 'Argentina', 'Brasil', etc.
#define TAM_CONTINENTE 20 // nombre de los continentes -> 'America', Europa, etc.

typedef struct
{
    int anio;
    int trimestre;
    char codigoPais[TAM_CODIGO];
    char nombrePais[TAM_PAIS];
    char operacion;
    float monto;
} Transferencia;

int cargarTransferencias(const char *nombreArchivo, Transferencia *transferencias, int maxRegistros);

void mostrarTransferencia(const Transferencia *transferencia);

void mostrarTransferencias(const Transferencia *transferencias, int cantidad);

#endif
