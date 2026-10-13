#include <stdio.h>
#include "../Transferencias/transferencias.h"

int main()
{
    FILE* pTransferencia=AbrirArch(ARCH_TRANSFERENCIA,"rt");
    FILE* pTransferencia_BaseLarga;
    if(!pTransferencia)
        return 1;

    Vector* infoT=vectorCrear(TAM_VEC,sizeof(tTransferencia));
    Vector* infoBS=vectorCrear(TAM_VEC_MIN,sizeof(tBase_Larga));

    CargarInfoTransferencia(infoT,pTransferencia);

    //GenerarArch_SaldoPorPais(infoT);
    //GenerarArch_SaldoPorContinente(infoT);
    //GenerarArch_SaldoTrimestre(infoT);
    //GenerarArch_SaldoAnio(infoT);
    //GenerarArch_SaldoPais_SegunOperacion(infoT);
    //GenerarArch_SaldoContinentePorcentaje(infoT);
    GenerarArch_BaseLarga(infoT);

    pTransferencia_BaseLarga=AbrirArch(ARCH_BASE_LARGA,"rt");
    if(!pTransferencia_BaseLarga)
        return 1;
    
    CargarInfo_BaseLarga(infoBS,pTransferencia_BaseLarga);

    GenerarArch_Promedio_Pais_Trimestre(infoBS);



    CerrarArch(pTransferencia);
    CerrarArch(pTransferencia_BaseLarga);
    
    vectorDestruir(infoT);
    vectorDestruir(infoBS);

    return 0;

}

