#include <stdio.h>
#include "../Transferencias/transferencias.h"

int main()
{
    FILE* pTransferencia=AbrirArch(ARCH_TRANSFERENCIA,"rt");
    if(!pTransferencia)
        return 1;

    Vector* infoT=vectorCrear(TAM_VEC,sizeof(tTransferencia));
    CargarInfoTransferencia(infoT,pTransferencia);
    vectorOrdenar(infoT,Cmp_T,1);

    GenerarArch_SaldoPorPais(infoT);
    GenerarArch_SaldoPorContinente(infoT);

    CerrarArch(pTransferencia);
    vectorDestruir(infoT);

    return 0;

}

