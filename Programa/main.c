#include <stdio.h>
#include "transferencias.h"

int main()
{
    FILE* pTransferencia=AbrirArch(ARCH_TRANSFERENCIA,"rt");

    if(!pTransferencia)
        return 1;



    Vector* infoT=vectorCrear(TAM_VEC,sizeof(tTransferencia));
    CargarInfoTransferencia(infoT,pTransferencia);
    vectorOrdenar(infoT,Cmp_T,1);

    GenerarArch_SaldoPorPais(infoT);

    CerrarArch(pTransferencia);

    return 0;

}
