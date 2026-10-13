#include <stdio.h>
#include "../Transferencias/transferencias.h"
int Menu_Principal();

int main()
{
    Menu_Principal();
    return 0;

}

int Menu_Principal()
{
    FILE* pTransferencia=AbrirArch(ARCH_TRANSFERENCIA,"rt");
    FILE* pTransferencia_BaseLarga;

    Vector* infoT= vectorCrear(TAM_VEC,sizeof(tTransferencia));
    Vector* infoBL= vectorCrear(TAM_VEC_MIN,sizeof(tBase_Larga));

    if(!pTransferencia)
        return 1;

    CargarInfoTransferencia(infoT,pTransferencia);
    CerrarArch(pTransferencia);

    GenerarArch_BaseLarga(infoT);

    pTransferencia_BaseLarga=AbrirArch(ARCH_BASE_LARGA,"rt");

    if(!pTransferencia_BaseLarga)
        return 1;

    CargarInfo_BaseLarga(infoBL,pTransferencia_BaseLarga);
    CerrarArch(pTransferencia_BaseLarga);

    int opcion;

    char respuesta_para_seguir[TAM_RESPUESTA]="Si";

    puts("\n\nMENU PRINCIPAL");
    puts("\n1. Totales por pais.");
    puts("\n2. Totales por continente.");
    puts("\n3. Saldo trimestral por pais.");
    puts("\n4. Totales anuales y variacion interanual.");
    puts("\n5. Ranking de paises.");
    puts("\n6. Participacion porcentual por continente.");
    puts("\n7. Generacion del archivo intermedio transferencias_base_larga.csv.");
    puts("\n8. Promedio trimestral por pais");
    puts("\n9. Promedio trimestral por continente, operacion y anio.");
    puts("\n10. Salir.");
    puts("\n");

    puts("\nIngrese una opcion: ");
    opcion=ValidarRango_Int(1,10);

    while(strcmpi(respuesta_para_seguir,"No")!=0)
    {
        switch(opcion)
        {
            case 1:
                    GenerarArch_SaldoPorPais(infoT);
                    break;
            case 2:
                    GenerarArch_SaldoPorContinente(infoT);
                    break;
            case 3:
                    GenerarArch_SaldoTrimestre(infoT);
                    break;

            case 4:
                    GenerarArch_SaldoAnio(infoT);
                    break;

            case 5:
                    GenerarArch_SaldoPais_SegunOperacion(infoT);
                    break;

            case 6:
                    GenerarArch_SaldoContinentePorcentaje(infoT);
                    break;

            case 7:
                    GenerarArch_BaseLarga(infoT);
                    break;

            case 8:
                    GenerarArch_Promedio_Pais_Trimestre(infoBL);
                    break;

            case 9:
                    GenerarArch_Promedio_Anio_Continente_Op(infoBL);
                    break;

            case 10:
                    strcpy(respuesta_para_seguir,"No");
                    break;
        }
        if(strcmpi(respuesta_para_seguir,"Si")==0)
        {
            puts("\nIngrese una opcion: ");
            opcion=ValidarRango_Int(1,10);
        }
    }
    vectorDestruir(infoT);
    vectorDestruir(infoBL);
    return 0;
}
