#include <stdio.h>
#include "../Transferencias/transferencias.h"

int main()
{
    FILE* pTransferencia=AbrirArch(ARCH_TRANSFERENCIA,"rt");
    if(!pTransferencia)
        return 1;

    Vector* infoT=vectorCrear(TAM_VEC,sizeof(tTransferencia));
    CargarInfoTransferencia(infoT,pTransferencia);
    

    GenerarArch_SaldoPorPais(infoT);
    GenerarArch_SaldoPorContinente(infoT);
    GenerarArch_SaldoTrimestre(infoT);

    CerrarArch(pTransferencia);
    
    vectorDestruir(infoT);

    return 0;

}

