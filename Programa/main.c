/* TP TOPICOS 2026*/

#include <stdio.h>
#include "../Transferencias/transferencias.h"
#include "../codigoError.h"

int Menu_Principal();
void MostrarMenu();
int CargarBaseLarga(Vector** infoBL);

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
    int codRet = TODO_OK;
    int opcion;

    char respuesta_para_seguir[TAM_RESPUESTA]="Si";

    Vector* infoT;
    /* la base larga se carga recien cuando se elige la opcion 8 o 9 */
    Vector* infoBL = NULL;

    if(!pTransferencia)
        return ERROR_ARCH;

    infoT = vectorCrear(TAM_VEC,sizeof(tTransferencia));

    if(infoT == NULL)
    {
        fclose(pTransferencia);
        return SIN_MEM;
    }

    codRet = CargarInfoTransferencia(infoT,pTransferencia);
    fclose(pTransferencia);

    if (codRet != TODO_OK)
    {
        vectorDestruir(infoT);
        return codRet;
    }

    MostrarMenu();
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
                    codRet = CargarBaseLarga(&infoBL);
                    if(codRet == TODO_OK)
                        codRet = GenerarArch_Promedio_Pais_Trimestre(infoBL);
                    break;

            case 9:
                    codRet = CargarBaseLarga(&infoBL);
                    if(codRet == TODO_OK)
                        codRet = GenerarArch_Promedio_Anio_Continente_Op(infoBL);
                    break;

            case 10:
                    strcpy(respuesta_para_seguir,"No");
                    puts("Programa finalizado correctamente\n");
                    break;
        }
        if(codRet == SIN_BASE_LARGA)
        {
            puts("\nPrimero genere el archivo intermedio con la opcion 7.");
            codRet = TODO_OK;
        }

        if(codRet != TODO_OK)
            break;

        if(compararSinMayusculas(respuesta_para_seguir,"Si")==0)
        {
            MostrarMenu();
            opcion=ValidarRango_Int(1,10);
        }
    }
    vectorDestruir(infoT);
    vectorDestruir(infoBL);
    return codRet;
}

void MostrarMenu()
{
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
}

int CargarBaseLarga(Vector** infoBL)
{
    FILE* pBaseLarga;
    int codRet;

    if(*infoBL != NULL)
        return TODO_OK;

    pBaseLarga=AbrirArch(ARCH_BASE_LARGA,"rt");
    if(!pBaseLarga)
        return SIN_BASE_LARGA;

    *infoBL = vectorCrear(TAM_VEC_MIN,sizeof(tBase_Larga));
    if(*infoBL == NULL)
    {
        fclose(pBaseLarga);
        return SIN_MEM;
    }

    codRet = CargarInfo_BaseLarga(*infoBL,pBaseLarga);
    fclose(pBaseLarga);

    return codRet;
}
