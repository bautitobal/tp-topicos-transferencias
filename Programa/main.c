#include <stdio.h>
#include "../Transferencias/transferencias.h"
#include "../codigoError.h"

int Menu_Principal();

int main()
{
    int codRet = Menu_Principal();

    if (codRet == ERROR_ARCH)
        printf("Error al trabajar con archivos\n");
    if (codRet == SIN_MEM)
        printf("Error: memoria insuficiente\n");

    return codRet;

}

int Menu_Principal()
{
    FILE* pTransferencia=AbrirArch(ARCH_TRANSFERENCIA,"rt");
    FILE* pTransferencia_BaseLarga;
    int codRet = TODO_OK;

    Vector* infoT;
    Vector* infoBL;

    if(!pTransferencia)
        return ERROR_ARCH;

    infoT = vectorCrear(TAM_VEC,sizeof(tTransferencia));

    if(infoT == NULL)
    {
        fclose(pTransferencia);
        return SIN_MEM;
    }

    infoBL = vectorCrear(TAM_VEC_MIN,sizeof(tBase_Larga));

    if(infoBL == NULL)
    {
        vectorDestruir(infoT);
        fclose(pTransferencia);
        return SIN_MEM;
    }

    if(!pTransferencia)
        return ERROR_ARCH;

    codRet = CargarInfoTransferencia(infoT,pTransferencia);
    fclose(pTransferencia);

    if (codRet != TODO_OK)
    {
        vectorDestruir(infoT);
        vectorDestruir(infoBL);    
        return codRet;
    }

    codRet = GenerarArch_BaseLarga(infoT);

    if (codRet != TODO_OK)
    {
        vectorDestruir(infoT);
        vectorDestruir(infoBL);    
        return codRet;
    }

    pTransferencia_BaseLarga=AbrirArch(ARCH_BASE_LARGA,"rt");

    if(!pTransferencia_BaseLarga)
        return ERROR_ARCH;

    codRet = CargarInfo_BaseLarga(infoBL,pTransferencia_BaseLarga);
    fclose(pTransferencia_BaseLarga);

    if(codRet != TODO_OK)
    {
        vectorDestruir(infoT);
        vectorDestruir(infoBL);
        return codRet;
    }

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

    printf("\nIngrese una opcion (10 para salir): ");
    opcion=ValidarRango_Int(1,10);

    while(compararSinMayusculas(respuesta_para_seguir,"No")!=0)
    {
        switch(opcion)
        {
            case 1:
                    codRet = GenerarArch_SaldoPorPais(infoT);
                    break;
            case 2:
                    codRet = GenerarArch_SaldoPorContinente(infoT);
                    break;
            case 3:
                    codRet = GenerarArch_SaldoTrimestre(infoT);
                    break;

            case 4:
                    codRet = GenerarArch_SaldoAnio(infoT);
                    break;

            case 5:
                    codRet = GenerarArch_SaldoPais_SegunOperacion(infoT);
                    break;

            case 6:
                    codRet = GenerarArch_SaldoContinentePorcentaje(infoT);
                    break;

            case 7:
                    codRet = GenerarArch_BaseLarga(infoT);
                    break;

            case 8:
                    codRet = GenerarArch_Promedio_Pais_Trimestre(infoBL);
                    break;

            case 9:
                    codRet = GenerarArch_Promedio_Anio_Continente_Op(infoBL);
                    break;

            case 10:
                    strcpy(respuesta_para_seguir,"No");
                    puts("Programa finalizado correctamente\n");
                    break;
        }
        if(codRet != TODO_OK)
            break;

        if(compararSinMayusculas(respuesta_para_seguir,"Si")==0)
        {
            printf("\nIngrese una opcion (10 para salir): ");
            opcion=ValidarRango_Int(1,10);
        }
    }
    vectorDestruir(infoT);
    vectorDestruir(infoBL);
    return codRet;
}
