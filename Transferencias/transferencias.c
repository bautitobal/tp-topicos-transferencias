#include "transferencias.h"


void trozarCampoVariable_Pais(tPais* info,char* linea)
{

    char* aux;

    aux=strchr(linea,'\n');

    *aux='\0';
    aux=strrchr(linea,';');
    strcpy(info->continente,aux+1);

    *aux='\0';
    aux=strrchr(linea,';');
    strcpy(info->desc_pais,aux+1);

    *aux='\0';
    strcpy(info->cod_pais,linea);

}
FILE* AbrirArch(const char* nombre, const char* modo)
{    
    FILE* pf=fopen(nombre, modo);

    if(!pf)
    {
        printf("\nError al abrir el archivo %s\n", nombre);
        return NULL;
    }
    
    return pf;
}

void trozarCampoVariable_Transferencia(tTransferencia* info, char* linea)
{
    char* aux;
    tTransferencia* infoT=(tTransferencia*)info;


    aux=strchr(linea,'\n');

    *aux='\0';
    aux=strrchr(linea,';');
    sscanf(aux+1,"%f",&infoT->monto);

    *aux='\0';
    aux=strrchr(linea,';');
    sscanf(aux+1,"%c",&infoT->operacion);

    *aux='\0';
    aux=strrchr(linea,';');
    strcpy(infoT->desc_pais,aux+1);

    *aux='\0';
    aux=strrchr(linea,';');
    strcpy(infoT->cod_pais,aux+1);

    *aux='\0';
    aux=strrchr(linea,';');
    sscanf(aux+1,"%d",&infoT->trimestre);

    *aux='\0';
    sscanf(linea,"%d",&infoT->anio);

}

int CargarInfoTransferencia(Vector* infoT, FILE* pTransferencia)
{
    tTransferencia aux;
    char linea_T[TAM_LINEA];
    int codRet;

    fgets(linea_T,TAM_LINEA,pTransferencia);

    while(fgets(linea_T,TAM_LINEA,pTransferencia))
    {
        trozarCampoVariable_Transferencia(&aux,linea_T);
        codRet = vectorInsertarFinal(infoT,&aux);

        if (codRet != TODO_OK)
            return codRet;
    }

    codRet = TODO_OK;
    return codRet;
}

int GenerarArch_SaldoPorPais(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_TRANSTOTAL_PAISES,"wt");

    tTransferencia* vecTransferencia;

    int i=0;
    int fin=vectorGetCantElem(infoT);
    int finPais;
    int codRet;

    Vector* s_pais;

    tSaldoPais aux;
    tSaldoPais* pais_aux;

    int cantRegistros,j;
    float debito,credito;
    char codActual[TAM_COD];
    char descActual[TAM_DESC_PAIS];

    if(!pf)
        return ERROR_ARCH;

    vectorOrdenar(infoT,Cmp_TCod,1);

    s_pais=vectorCrear(CANT_PAISES,sizeof(tSaldoPais));
    if (s_pais == NULL)
    {
        fclose(pf);
        return SIN_MEM;
    }

    vecTransferencia=vectorGetElementoPorPos(infoT,i);

    while(i<fin)
    {
        cantRegistros=0;
        debito=0;
        credito=0;
        strcpy(codActual,vecTransferencia->cod_pais);
        strcpy(descActual,vecTransferencia->desc_pais);
        while(i < fin && (compararSinMayusculas(codActual, vecTransferencia->cod_pais) == 0))
        {
            cantRegistros++;
            
            if(vecTransferencia->operacion == DEBITO)
            debito += vecTransferencia->monto;
            else
            credito += vecTransferencia->monto;
            
            vecTransferencia=vectorGetElementoPorPos(infoT,++i);
            
        }
        aux.cantRegistros=cantRegistros;
        strcpy(aux.cod_pais,codActual);
        strcpy(aux.desc_pais,descActual);
        aux.credito=credito;
        aux.debito=debito;

        codRet = vectorInsertarFinal(s_pais,&aux);

        if (codRet != TODO_OK)
        {
            vectorDestruir(s_pais);
            fclose(pf);
            return codRet;
        }

    }
    finPais=vectorGetCantElem(s_pais);


    vectorOrdenar(s_pais,cmp_CreditoPais,2);

    fprintf(pf,"PAIS_COD;PAIS_DESC;registros;total_credito;total_debito\n");
    printf("\n%-9s %-40s %10s %14s %14s\n","PAIS_COD","PAIS_DESC","registros","total_credito","total_debito");
    for(j=0;j<finPais;j++)
    {
        pais_aux=vectorGetElementoPorPos(s_pais,j);
        fprintf(pf,"%s;%s;%d;%.2f;%.2f\n",pais_aux->cod_pais,pais_aux->desc_pais,
        pais_aux->cantRegistros,pais_aux->credito,pais_aux->debito);
        printf("%-9s %-40s %10d %14.2f %14.2f\n",pais_aux->cod_pais,pais_aux->desc_pais,
        pais_aux->cantRegistros,pais_aux->credito,pais_aux->debito);
    }
    fclose(pf);
    vectorDestruir(s_pais);

    codRet = TODO_OK;
    return codRet;

}
int GenerarArch_SaldoPorContinente(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_SALDO_CONTINENTE,"wt");
    FILE* pPais=AbrirArch(ARCH_PAISES,"rt");

    tTransferencia* vec;
    tPais Pais_aux;
    tSaldoContinente* saldo;
    Vector* s_cont;

    char linea[TAM_LINEA];
    int i;
    int j;
    int codRet;
    int ceInfoT=vectorGetCantElem(infoT);

    if(!pf || !pPais)
    {
        if(pf)
            fclose(pf);
        if(pPais)
            fclose(pPais);
        return ERROR_ARCH;
    }

    s_cont=vectorCrear(CANT_CONTINENTE,sizeof(tSaldoContinente));

    if (s_cont == NULL)
    {
        fclose(pf);
        fclose(pPais);
        return SIN_MEM;
    }

    codRet = InicializarVectorSaldoContinente(s_cont);

    if(codRet != TODO_OK)
    {
        vectorDestruir(s_cont);
        fclose(pPais);
        fclose(pf);
        return codRet;
    }

    for(j=0;j<ceInfoT;j++)
    {
        vec=vectorGetElementoPorPos(infoT,j);
        rewind(pPais);
        fgets(linea,TAM_LINEA,pPais);
        while(fgets(linea,TAM_LINEA,pPais))
        {
            trozarCampoVariable_Pais(&Pais_aux,linea);
            if(compararSinMayusculas(Pais_aux.cod_pais,vec->cod_pais)==0)
                AcumularSaldosSegunContinente(s_cont,&Pais_aux,vec);
        }
    }

    vectorOrdenar(s_cont,CmpCredito_Continente,2);
    fprintf(pf,"CONTINENTE;Registros;Total_Credito;Total_Debito\n");
    printf("\n%-16s %10s %14s %14s\n","CONTINENTE","Registros","Total_Credito","Total_Debito");

    for(i=0;i<vectorGetCantElem(s_cont);i++)
    {
        saldo = vectorGetElementoPorPos(s_cont,i);
        fprintf(pf,"%s;%d;%.2f;%.2f\n", saldo->continente, saldo->cantRegistros,
                                        saldo->credito, saldo->debito);
        printf("%-16s %10d %14.2f %14.2f\n", saldo->continente, saldo->cantRegistros,
                                        saldo->credito, saldo->debito);
    }
    fclose(pf);
    fclose(pPais);
    vectorDestruir(s_cont);

    return TODO_OK;
}
int InicializarVectorSaldoContinente(Vector* vec)
{
    FILE* pPaises=AbrirArch(ARCH_PAISES,"rt");

    char linea[TAM_LINEA];

    tPais infoP;
    tSaldoContinente nuevo;
    tSaldoContinente* saldo;
    int i;
    bool existe;
    int codRet;

    if(!pPaises)
        return ERROR_ARCH;

    fgets(linea,TAM_LINEA,pPaises);

    while(fgets(linea,TAM_LINEA,pPaises))
    {
        trozarCampoVariable_Pais(&infoP,linea);
        existe = false;

        for(i=0; i < vectorGetCantElem(vec); i++)
        {
            saldo = vectorGetElementoPorPos(vec, i);
            if(compararSinMayusculas(saldo->continente, infoP.continente) == 0)
            {
                existe = true;
                i = vectorGetCantElem(vec);
            }
        }

        if(!existe)
        {
            strcpy(nuevo.continente, infoP.continente);
            nuevo.cantRegistros = 0;
            nuevo.credito = 0;
            nuevo.debito = 0;
            codRet = vectorInsertarFinal(vec, &nuevo);
        }
    }
    fclose(pPaises);
    return TODO_OK;
}
void AcumularSaldosSegunContinente(Vector* vecSaldoContinente, tPais* infoPais,tTransferencia* vecTransferencia)
{
    int i;
    for(i=0; i < vectorGetCantElem(vecSaldoContinente); i++)
    {
        tSaldoContinente* saldo = vectorGetElementoPorPos(vecSaldoContinente, i);
        if(compararSinMayusculas(infoPais->continente, saldo->continente) == 0)
        {
            saldo->cantRegistros++;
            if(vecTransferencia->operacion == CREDITO)
            {
                saldo->credito += vecTransferencia->monto;
            }
            else
            {
                saldo->debito += vecTransferencia->monto;
            }
            return;
        }
    }
}
int GenerarArch_SaldoTrimestre(Vector* infoT)
{
    FILE* pf = AbrirArch(ARCH_SALDO_TRIMESTRE, "wt");

    int anio;

    int i=0;
    int ce=vectorGetCantElem(infoT);

    bool tieneOperacion;

    char paisActual[TAM_DESC_PAIS];
    char codActual[TAM_COD];

    float vecSaldoTrimestre[TAM_TRIMESTRE];

    tTransferencia* vec;

    if(!pf)
        return ERROR_ARCH;

    printf("\nIngrese un anio para mostrar en la tabla (2015-Actualidad): ");
    anio = ValidarRango_Int(ANIO_MENOR, ANIO_ACTUAL);
    vectorOrdenar(infoT, Cmp_TDesc, 1);

    fprintf(pf, "%-40s %8s %8s %8s %8s\n","Pais", "T1", "T2", "T3", "T4");
    printf("\n%-40s %8s %8s %8s %8s\n","Pais", "T1", "T2", "T3", "T4");
    vec=vectorGetElementoPorPos(infoT,i);

    while(i<ce)
    {
        
        strcpy(codActual, vec->cod_pais);
        strcpy(paisActual, vec->desc_pais);
        tieneOperacion = false;
        PonerEnCeroVecFloat(vecSaldoTrimestre);
        
        while(i<ce && compararSinMayusculas(paisActual, vec->desc_pais) == 0)
        {
            if(vec->anio == anio)
                AcumularSaldoTrimestre(vec,vecSaldoTrimestre,&tieneOperacion);

            vec=vectorGetElementoPorPos(infoT,++i);
            
        }

        if(tieneOperacion)
        {
            fprintf(pf,"%-40s %8.2f %8.2f %8.2f %8.2f\n", paisActual,
                    vecSaldoTrimestre[0], vecSaldoTrimestre[1],
                    vecSaldoTrimestre[2], vecSaldoTrimestre[3]);
            printf("%-40s %8.2f %8.2f %8.2f %8.2f\n", paisActual,
                    vecSaldoTrimestre[0], vecSaldoTrimestre[1],
                    vecSaldoTrimestre[2], vecSaldoTrimestre[3]);
        }
    }

    fclose(pf);
    return TODO_OK;
}



int cmp_CreditoPais(void* a, void* b)
{
    tSaldoPais* x= (tSaldoPais*)a;
    tSaldoPais* y= (tSaldoPais*)b;
    return (x->credito > y->credito) - (x->credito < y->credito);
}
int Cmp_TCod(void* a, void* b)
{
    tTransferencia* x= (tTransferencia*)a;
    tTransferencia* y= (tTransferencia*)b;
    return compararSinMayusculas(x->cod_pais,y->cod_pais);
}
int CmpCredito_Continente(void* a, void* b)
{
    tSaldoContinente* x=(tSaldoContinente*)a;
    tSaldoContinente* y=(tSaldoContinente*)b;
    return (x->credito > y->credito) - (x->credito < y->credito);
}
int ValidarRango_Int(int LimI, int LimS)
{
    int dato;
    int leidos;
    int c;

    leidos=scanf("%d",&dato);
    while(leidos!=EOF && (leidos!=1 || dato<LimI || dato>LimS))
    {
        /* descarta lo que quedo en el buffer (por ejemplo, letras) */
        while((c=getchar())!='\n' && c!=EOF);

        printf("\nError - Ingrese un numero entre %d y %d: ",LimI, LimS);
        leidos=scanf("%d",&dato);
    }

    /* sin mas entrada disponible se toma el limite superior */
    if(leidos==EOF)
        dato=LimS;

    return dato;
}
void PonerEnCeroVecFloat(float* vec)
{
    int i;
    float* aux=vec;

    for(i=0;i<TAM_TRIMESTRE;i++)
    {
        *aux=0;
        aux++;
    }
}
void AcumularSaldoTrimestre(tTransferencia* infoT, float* saldoTrimestre,bool* tieneOperacion)
{
    tTransferencia* auxT=infoT;
    switch(auxT->trimestre)
    {
        case 1:
                if(auxT->operacion == CREDITO)
                    saldoTrimestre[0] += auxT->monto;
                else
                    saldoTrimestre[0] -= auxT->monto;
                break;
        case 2:
                if(auxT->operacion == CREDITO)
                    saldoTrimestre[1] += auxT->monto;
                else
                    saldoTrimestre[1] -= auxT->monto;
                break;

        case 3:
                if(auxT->operacion == CREDITO)
                    saldoTrimestre[2] += auxT->monto;
                else
                    saldoTrimestre[2] -= auxT->monto;
                break;

        case 4:
                if(auxT->operacion == CREDITO)
                    saldoTrimestre[3] += auxT->monto;
                else
                    saldoTrimestre[3] -= auxT->monto;
                break;
    }
    if(auxT->operacion==CREDITO || auxT->operacion==DEBITO)
        *tieneOperacion=true;

}
int GenerarArch_SaldoAnio(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_SALDO_ANIO,"wt");

    tTransferencia* vecT;
    /*tTransferencia* pfin=vecT+infoT->ce;*/

    float total_debito, total_credito, variacion,saldoActual, saldoAnterior=0;

    int anioActual;
    int i=0;
    int ce=vectorGetCantElem(infoT);

    if(!pf)
        return ERROR_ARCH;

    vectorOrdenar(infoT,Cmp_TAnio,1);

    fprintf(pf,"ANIO;C;D;SALDO;VARIACION\n");
    printf("\n%-6s %12s %12s %12s %12s\n","ANIO","C","D","SALDO","VARIACION");

    vecT=vectorGetElementoPorPos(infoT,i);

    while(i<ce)
    {
        anioActual=vecT->anio;
        total_credito=0;
        total_debito=0;
        saldoActual=0;
        variacion=0;

        while(i<ce && anioActual==vecT->anio)
        {
            if(vecT->operacion==CREDITO)
            {
                total_credito+=vecT->monto;
            }
            else
            {
                total_debito+=vecT->monto;
            }
            vecT=vectorGetElementoPorPos(infoT,++i);
           
        }

        saldoActual=total_credito-total_debito;

        if(anioActual==ANIO_MENOR)
        {
            fprintf(pf,"%d;%.2f;%.2f;%.2f;%s\n",anioActual,total_credito,total_debito,saldoActual,SIN_VARIACION);
            printf("%-6d %12.2f %12.2f %12.2f %12s\n",anioActual,total_credito,total_debito,saldoActual,SIN_VARIACION);
        }
        else
        {
            variacion= ((saldoActual-saldoAnterior)/saldoAnterior)*100;
            fprintf(pf,"%d;%.2f;%.2f;%.2f;%.2f\n",anioActual,total_credito,total_debito,saldoActual,variacion);
            printf("%-6d %12.2f %12.2f %12.2f %12.2f\n",anioActual,total_credito,total_debito,saldoActual,variacion);
        }
        saldoAnterior=saldoActual;
    }
    fclose(pf);
    return TODO_OK;
}
int GenerarArch_SaldoPais_SegunOperacion(Vector* infoT)
{
    int codRet;
    FILE* pf= AbrirArch(ARCH_SALDO_PAISES_OPERACION,"wt");

    Vector* infoP;

    tSaldoPais* vecP;
    tSaldoPais actual;

    tTransferencia* vecT;

    int cantPaisesListar,i,j=0;
    int ceTransferencia=vectorGetCantElem(infoT);
    char opcionOperacion;

    if(!pf)
        return ERROR_ARCH;

    vectorOrdenar(infoT,Cmp_TDesc,MENOR_A_MAYOR);

    infoP=vectorCrear(CANT_PAISES,sizeof(tSaldoPais));

    if (infoP == NULL)
    {
        fclose(pf);
        return SIN_MEM;
    }

    printf("\nIngrese la cantidad de paises para listar (1-%d): ",CANT_PAISES);
    cantPaisesListar=ValidarRango_Int(1,CANT_PAISES);
    printf("\nIngrese una operacion - Credito (%c) o Debito (%c): ",CREDITO,DEBITO);
    opcionOperacion=ValidarChar(CREDITO,DEBITO);

    vecT=vectorGetElementoPorPos(infoT,j);
    while(j<ceTransferencia)
    {
        strcpy(actual.desc_pais,vecT->desc_pais);
        strcpy(actual.cod_pais,vecT->cod_pais);
        actual.cantRegistros=0;
        actual.credito=0;
        actual.debito=0;

        while(j<ceTransferencia && compararSinMayusculas(actual.desc_pais,vecT->desc_pais)==0)
        {
            actual.cantRegistros++;
            if(vecT->operacion==CREDITO)
            {
                actual.credito+=vecT->monto;
            }
            else
            {
                actual.debito+=vecT->monto;
            }
            vecT=vectorGetElementoPorPos(infoT,++j);
        }
        codRet = vectorInsertarFinal(infoP,&actual);

        if (codRet != TODO_OK)
        {
            vectorDestruir(infoP);
            fclose(pf);
            return codRet;
        }

    }
    switch(opcionOperacion)
    {
        case CREDITO:
                vectorOrdenar(infoP,Cmp_SPCredito,MAYOR_A_MENOR);
                break;
        case DEBITO:
                vectorOrdenar(infoP,Cmp_SPDebito,MAYOR_A_MENOR);
                break;
    }

    if(cantPaisesListar > (int)vectorGetCantElem(infoP))
        cantPaisesListar = vectorGetCantElem(infoP);

    fprintf(pf,"PUESTO;PAIS_COD;PAIS_DESC;registros;total_credito;total_debito\n");
    printf("\n%-7s %-9s %-40s %10s %14s %14s\n","PUESTO","PAIS_COD","PAIS_DESC","registros",
                                                "total_credito","total_debito");
    for(i=0;i<cantPaisesListar;i++)
    {
        vecP=vectorGetElementoPorPos(infoP,i);
        fprintf(pf,"%d;%s;%s;%d;%.2f;%.2f\n",i+1,vecP->cod_pais,vecP->desc_pais,vecP->cantRegistros,
                                            vecP->credito,vecP->debito);
        printf("%-7d %-9s %-40s %10d %14.2f %14.2f\n",i+1,vecP->cod_pais,vecP->desc_pais,
                                            vecP->cantRegistros,vecP->credito,vecP->debito);
    }
    fclose(pf);
    vectorDestruir(infoP);
    return TODO_OK;
}
int GenerarArch_SaldoContinentePorcentaje(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_SALDO_CONTINENTE_PORCENTAJE,"w+b");
    FILE* pPais=AbrirArch(ARCH_PAISES,"rt");

    Vector* infoCPorcentaje;
    Vector* s_cont;

    tTransferencia* vecT;

    int i;
    int ceTransferencia=vectorGetCantElem(infoT);
    int j;
    int codRet;

    tPais Pais_aux;

    tSaldoContinente* saldoContinente;

    tPorcentajeContinente PC_Aux;
    tPorcentajeContinente* pPC_Aux;

    float TotalDebito=0,TotalCredito=0;

    char linea[TAM_LINEA];

    if(!pf || !pPais)
    {
        if(pf)
            fclose(pf);
        if(pPais)
            fclose(pPais);
        return ERROR_ARCH;
    }

    infoCPorcentaje=vectorCrear(CANT_CONTINENTE,sizeof(tPorcentajeContinente));
    s_cont= vectorCrear(CANT_CONTINENTE,sizeof(tSaldoContinente));

    if (infoCPorcentaje == NULL || s_cont == NULL)
    {
        vectorDestruir(infoCPorcentaje);
        vectorDestruir(s_cont);

        fclose(pPais);
        fclose(pf);
        return SIN_MEM;
    }

    codRet = InicializarVectorSaldoContinente(s_cont);

    if (codRet != TODO_OK)
    {
        vectorDestruir(infoCPorcentaje);
        vectorDestruir(s_cont);
        fclose(pPais);
        fclose(pf);
        return codRet;
    }

    CalcularTotalDeOperaciones(infoT,&TotalDebito,&TotalCredito);

    for(i=0;i<ceTransferencia;i++)
    {
        rewind(pPais);
        fgets(linea,TAM_LINEA,pPais);
        while(fgets(linea,TAM_LINEA,pPais))
        {
            vecT=vectorGetElementoPorPos(infoT,i);
            trozarCampoVariable_Pais(&Pais_aux,linea);
            if(compararSinMayusculas(Pais_aux.cod_pais,vecT->cod_pais)==0)
                AcumularSaldosSegunContinente(s_cont,&Pais_aux,vecT);

        }
    }

    for(j=0;j<s_cont->ce;j++)
    {
        saldoContinente=vectorGetElementoPorPos(s_cont,j);
        strcpy(PC_Aux.continente,saldoContinente->continente);
        if(TotalCredito!=0 && TotalDebito!=0)
        {
            PC_Aux.credito_porcentaje= ((saldoContinente->credito)/TotalCredito)*100;
            PC_Aux.debito_porcentaje= ((saldoContinente->debito)/TotalDebito)*100;
        }
        else
        {
            PC_Aux.credito_porcentaje=0;
            PC_Aux.debito_porcentaje=0;
        }

        codRet = vectorInsertarFinal(infoCPorcentaje,&PC_Aux);
        if(codRet != TODO_OK)
            break;
        
    }

    vectorOrdenar(infoCPorcentaje,Cmp_Porc_Continente,MENOR_A_MAYOR);

    for(j=0;j<vectorGetCantElem(infoCPorcentaje);j++)
    {
        pPC_Aux=vectorGetElementoPorPos(infoCPorcentaje,j);
        fwrite(pPC_Aux,sizeof(tPorcentajeContinente),1,pf);
        
    }

    fclose(pf);
    fclose(pPais);

    vectorDestruir(infoCPorcentaje);
    vectorDestruir(s_cont);

    return LeerArchivo_PorcentajeContinente();
}
int LeerArchivo_PorcentajeContinente()
{
    FILE* pf=AbrirArch(ARCH_SALDO_CONTINENTE_PORCENTAJE,"rb");

    char opcionParaLeer;

    tPorcentajeContinente PC_Aux;

    if (!pf)
        return ERROR_ARCH;

    printf("\nDesea leer el archivo? Si (%c) o N (%c): ",AFIRMATIVO,NEGATIVO);
    opcionParaLeer=ValidarChar(AFIRMATIVO,NEGATIVO);

        if(opcionParaLeer==AFIRMATIVO)
    {
        printf("%-30s %8s %8s\n","CONTINENTE","C","D");
        fread(&PC_Aux,sizeof(PC_Aux),1,pf);
        while(!feof(pf))
        {
            printf("%-30s %8.2f %8.2f\n",PC_Aux.continente,PC_Aux.credito_porcentaje,PC_Aux.debito_porcentaje);
            fread(&PC_Aux,sizeof(PC_Aux),1,pf);
        }
    }
    fclose(pf);
    return TODO_OK;
}
int GenerarArch_BaseLarga(Vector* infoT)
{
    FILE* pBaseLarga = AbrirArch(ARCH_BASE_LARGA,"wt");
    FILE* pPais=AbrirArch(ARCH_PAISES,"rt");

    tTransferencia* vecT;
    tTransferencia auxT;

    int i=0;
    int ceTransferencia=vectorGetCantElem(infoT);

    char continente[TAM_CONTINENTE];

    if(!pBaseLarga || !pPais)
    {
        if(pBaseLarga)
            fclose(pBaseLarga);
        if(pPais)
            fclose(pPais);
        return ERROR_ARCH;
    }

    vectorOrdenar(infoT,Cmp_Transferencia_VariosCampos,MENOR_A_MAYOR);

    fprintf(pBaseLarga,"ANIO;TRIMESTRE;PAIS_COD;PAIS_DESC;CONTINENTE;OPERACION;MONTO\n");
    vecT=vectorGetElementoPorPos(infoT,i);

    while(i<ceTransferencia)
    {
        auxT.anio=vecT->anio;
        strcpy(auxT.cod_pais,vecT->cod_pais);
        auxT.trimestre=vecT->trimestre;
        auxT.operacion=vecT->operacion;
        strcpy(auxT.desc_pais,vecT->desc_pais);
        auxT.monto=0;

        while(i<ceTransferencia &&  auxT.anio==vecT->anio && auxT.trimestre==vecT->trimestre
                    && auxT.operacion==vecT->operacion  && compararSinMayusculas(auxT.cod_pais,vecT->cod_pais)==0)
        {
            auxT.monto+=vecT->monto;
            vecT=vectorGetElementoPorPos(infoT,++i);
        }
        EncontrarContinente_SegunPais(continente,&auxT,pPais);
        fprintf(pBaseLarga,"%d;%d;%s;%s;%s;%c;%.2f\n",auxT.anio,auxT.trimestre,auxT.cod_pais,auxT.desc_pais,continente,auxT.operacion,auxT.monto);
    }
    fclose(pBaseLarga);
    fclose(pPais);

    return TODO_OK;
}
void EncontrarContinente_SegunPais(char* continente, tTransferencia* auxT, FILE* pPais)
{
    bool encontrado=false;
    tPais vecP;
    char linea[TAM_LINEA];

    rewind(pPais);
    fgets(linea,TAM_LINEA,pPais);

    while(!encontrado && fgets(linea,TAM_LINEA,pPais) )
    {
        trozarCampoVariable_Pais(&vecP,linea);
        if(compararSinMayusculas(vecP.cod_pais,auxT->cod_pais)==0)
        {
            strcpy(continente,vecP.continente);
            encontrado=true;
        }
    }


}

int Cmp_TDesc(void* a, void* b)
{
    tTransferencia* x = (tTransferencia*)a;
    tTransferencia* y = (tTransferencia*)b;
    return compararSinMayusculas(x->desc_pais, y->desc_pais);
}
int Cmp_TAnio(void* a, void* b)
{
    tTransferencia* x= (tTransferencia*)a;
    tTransferencia* y= (tTransferencia*)b;
    return x->anio - y->anio;
}
int Cmp_SPDebito(void* a, void* b)
{
    tSaldoPais* x=(tSaldoPais*)a;
    tSaldoPais* y=(tSaldoPais*)b;
    return (x->debito > y->debito) - (x->debito < y->debito);
}
int Cmp_SPCredito(void* a, void* b)
{
    tSaldoPais* x=(tSaldoPais*)a;
    tSaldoPais* y=(tSaldoPais*)b;
    return (x->credito > y->credito) - (x->credito < y->credito);
}
int Cmp_Porc_Continente(void* a, void* b)
{
    tPorcentajeContinente* x = (tPorcentajeContinente*)a;
    tPorcentajeContinente* y = (tPorcentajeContinente*)b;
    return compararSinMayusculas(x->continente,y->continente);
}

int Cmp_Transferencia_VariosCampos(void* a, void* b)
{
    tTransferencia* t1 = (tTransferencia*)a;
    tTransferencia* t2 = (tTransferencia*)b;

    if(t1->anio != t2->anio)
        return t1->anio - t2->anio;

    if(t1->trimestre != t2->trimestre)
        return t1->trimestre - t2->trimestre;

    if(compararSinMayusculas(t1->cod_pais,t2->cod_pais)!=0)
        return compararSinMayusculas(t1->cod_pais,t2->cod_pais);

    if(t1->operacion != t2->operacion)
        return t1->operacion - t2->operacion;

    return 0;
}

char ValidarChar(char a, char b)
{
    char dato;
    scanf(" %c",&dato);
    while(dato!= a && dato!=b)
    {
        printf("\nError - La opcion es %c o %c: ",a,b);
        scanf(" %c",&dato);
    }
    return dato;
}
void CalcularTotalDeOperaciones(Vector* vecT, float* totalDebito, float* totalCredito)
{
    int i;
    tTransferencia* aux;
    for(i=0;i<vectorGetCantElem(vecT);i++)
    {
        aux=vectorGetElementoPorPos(vecT,i);

        if(aux->operacion==CREDITO)
            *totalCredito+=aux->monto;
        else
            *totalDebito+=aux->monto;

        
    }

}

void trozarCampoVariable_BaseLarga(tBase_Larga* infoBL,char* linea)
{
    char* aux;

    aux=strchr(linea,'\n');

    *aux='\0';
    aux=strrchr(linea,';');
    sscanf(aux+1,"%f",&infoBL->monto);

    *aux='\0';
    aux=strrchr(linea,';');
    sscanf(aux+1,"%c",&infoBL->operacion);

    *aux='\0';
    aux=strrchr(linea,';');
    strcpy(infoBL->continente,aux+1);

    *aux='\0';
    aux=strrchr(linea,';');
    strcpy(infoBL->desc_pais,aux+1);

    *aux='\0';
    aux=strrchr(linea,';');
    strcpy(infoBL->cod_pais,aux+1);

    *aux='\0';
    aux=strrchr(linea,';');
    sscanf(aux+1,"%d",&infoBL->trimestre);

    *aux='\0';
    sscanf(linea,"%d",&infoBL->anio);

}

int CargarInfo_BaseLarga(Vector* infoBS, FILE* pf)
{
    char linea[TAM_LINEA];
    tBase_Larga aux;
    int codRet;

    fgets(linea,TAM_LINEA,pf);

    while(fgets(linea,TAM_LINEA,pf))
    {
        trozarCampoVariable_BaseLarga(&aux,linea);
        codRet = vectorInsertarFinal(infoBS,&aux);

        if (codRet != TODO_OK)
            return codRet;
    }

    return TODO_OK;
}
int GenerarArch_Promedio_Pais_Trimestre(Vector* infoBL)
{
    FILE* pf=AbrirArch(ARCH_PROMEDIO_PAIS_TRIMESTRE,"wt");

    Vector* infoPPT;

    tPromedio_Pais_Trimestre auxPPT;
    tPromedio_Pais_Trimestre* pAux;

    tBase_Larga* vecBS;

    int i;
    int j=0;
    int ceBaseLarga=vectorGetCantElem(infoBL);
    int codRet;
    float promedio;

    if(!pf)
        return ERROR_ARCH;

    vectorOrdenar(infoBL,Cmp_BaseLarga_Pais_Operacion,MENOR_A_MAYOR);

    infoPPT=vectorCrear(TAM_VEC_MIN,sizeof(tPromedio_Pais_Trimestre));

    if (infoPPT == NULL)
    {
        fclose(pf);
        return SIN_MEM;
    }

    fprintf(pf,"PAIS_DESC;OPERACION;Suma_monto;Cant_trimestre;Promedio\n");
    printf("\n%-40s %10s %12s %15s %10s\n","PAIS_DESC","OPERACION","suma_monto","cant_trimestres","PROMEDIO");
    vecBS=vectorGetElementoPorPos(infoBL,j);

    while(j<ceBaseLarga)
    {
        strcpy(auxPPT.desc_pais,vecBS->desc_pais);
        auxPPT.operacion=vecBS->operacion;
        auxPPT.cant_trimestre=0;
        auxPPT.promedio_credito=0;
        auxPPT.promedio_debito=0;
        auxPPT.sumaMontos=0;
        while(j<ceBaseLarga && compararSinMayusculas(auxPPT.desc_pais,vecBS->desc_pais)==0 && auxPPT.operacion==vecBS->operacion)
        {
            if(vecBS->monto>0)
                auxPPT.cant_trimestre++;

            auxPPT.sumaMontos+=vecBS->monto;
            vecBS=vectorGetElementoPorPos(infoBL,++j);
        }
        if(auxPPT.cant_trimestre!=0)
        {
            if(auxPPT.operacion==CREDITO)
                auxPPT.promedio_credito=auxPPT.sumaMontos/auxPPT.cant_trimestre;
            else
                auxPPT.promedio_debito=auxPPT.sumaMontos/auxPPT.cant_trimestre;
        }
        codRet = vectorInsertarFinal(infoPPT,&auxPPT);

        if (codRet != TODO_OK)
        {
            vectorDestruir(infoPPT);
            fclose(pf);
            return codRet;
        }
    }
    vectorOrdenar(infoPPT,Cmp_PPT_DescPais,MENOR_A_MAYOR);
    pAux=(tPromedio_Pais_Trimestre*)infoPPT->vec;

    for(i=0;i<vectorGetCantElem(infoPPT);i++)
    {
        pAux=vectorGetElementoPorPos(infoPPT,i);

        if(pAux->operacion==CREDITO)
            promedio=pAux->promedio_credito;
        else
            promedio=pAux->promedio_debito;

        fprintf(pf,"%s;%c;%.2f;%d;%.2f\n",pAux->desc_pais,pAux->operacion,pAux->sumaMontos,pAux->cant_trimestre,promedio);
        printf("%-40s %10c %12.2f %15d %10.2f\n",pAux->desc_pais,pAux->operacion,pAux->sumaMontos,pAux->cant_trimestre,promedio);
    }
    fclose(pf);
    vectorDestruir(infoPPT);


    return TODO_OK;
}
int GenerarArch_Promedio_Anio_Continente_Op(Vector* infoBL)
{
    FILE* pf=AbrirArch(ARCH_PROMEDIO_CONTINENTE_ANI_OP,"wt");

    tBase_Larga* vecBL;
    tBase_Larga aux;

    int i=0;
    int ceBaseLarga=vectorGetCantElem(infoBL);
    int cant_registros;

    float promedio;

    if(!pf)
        return ERROR_ARCH;

    vectorOrdenar(infoBL,Cmp_Prom_ACO,MENOR_A_MAYOR);

    fprintf(pf,"ANIO;CONTINENTE;OPERACION;Suma_monto;cant_trimestre;PROMEDIO\n");
    printf("\n%-6s %-16s %10s %12s %15s %10s\n","ANIO","CONTINENTE","OPERACION","suma_monto","cant_trimestres","PROMEDIO");
    vecBL=vectorGetElementoPorPos(infoBL,i);

    while(i<ceBaseLarga)
    {
        aux.anio=vecBL->anio;
        strcpy(aux.continente,vecBL->continente);
        aux.operacion=vecBL->operacion;
        aux.monto=0;
        cant_registros=0;
        promedio=0;

        while(i<ceBaseLarga && aux.anio==vecBL->anio && aux.operacion==vecBL->operacion && compararSinMayusculas(aux.continente,vecBL->continente)==0)
        {
            aux.monto+=vecBL->monto;
            if(vecBL->monto>0)
                cant_registros++;
            vecBL=vectorGetElementoPorPos(infoBL,++i);
        }
        if(cant_registros!=0)
            promedio=aux.monto/cant_registros;

        fprintf(pf,"%d;%s;%c;%.2f;%d;%.2f\n",aux.anio,aux.continente,aux.operacion,aux.monto,cant_registros,promedio);
        printf("%-6d %-16s %10c %12.2f %15d %10.2f\n",aux.anio,aux.continente,aux.operacion,aux.monto,cant_registros,promedio);
    }
    fclose(pf);
    return TODO_OK;
}


int Cmp_BaseLarga_Pais_Operacion(void* a, void* b)
{
    tBase_Larga* x = (tBase_Larga*)a;
    tBase_Larga* y = (tBase_Larga*)b;

    if(compararSinMayusculas(x->desc_pais,y->desc_pais)!=0)
        return compararSinMayusculas(x->desc_pais,y->desc_pais);

    if(x->operacion - y->operacion)
        return x->operacion - y->operacion;

    return 0;

}

int Cmp_PPT_PromedioCredito(void* a, void* b)
{
    tPromedio_Pais_Trimestre* x = (tPromedio_Pais_Trimestre*)a;
    tPromedio_Pais_Trimestre* y = (tPromedio_Pais_Trimestre*)b;
    return (x->promedio_credito > y->promedio_credito) - (x->promedio_credito < y->promedio_credito);
}

int Cmp_PPT_DescPais(void* a, void* b)
{
    tPromedio_Pais_Trimestre* x = (tPromedio_Pais_Trimestre*)a;
    tPromedio_Pais_Trimestre* y = (tPromedio_Pais_Trimestre*)b;
    return compararSinMayusculas(x->desc_pais,y->desc_pais);
}
int Cmp_Prom_ACO(void* a, void* b)
{
    tBase_Larga* x = (tBase_Larga*)a;
    tBase_Larga* y = (tBase_Larga*)b;


    if(x->anio!=y->anio)
        return x->anio - y->anio;

    if(compararSinMayusculas(x->continente,y->continente)!=0)
        return compararSinMayusculas(x->continente,y->continente);

    if(x->operacion!=y->operacion)
            return x->operacion - y->operacion;

    return 0;

}

int compararSinMayusculas(const char* c1, const char* c2)
{
    while(*c1 && *c2 && tolower((unsigned char)*c1) == tolower((unsigned char)*c2))
    {
        c1++;
        c2++;
    }

    return tolower((unsigned char)*c1) - tolower((unsigned char)*c2);
}
