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
FILE* AbrirArch(const char* Archivo, const char* modo)
{
    FILE* pf=fopen(Archivo,modo);
    if(!pf)
        return NULL;
    return pf;
}
void CerrarArch(FILE* p)
{
    fclose(p);
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

void CargarInfoTransferencia(Vector* infoT, FILE* pTransferencia)
{
    tTransferencia aux;
    char linea_T[TAM_LINEA];

    fgets(linea_T,TAM_LINEA,pTransferencia);

    while(fgets(linea_T,TAM_LINEA,pTransferencia))
    {
        trozarCampoVariable_Transferencia(&aux,linea_T);
        vectorInsertarFinal(infoT,&aux);
    }


}

void GenerarArch_SaldoPorPais(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_TRANSTOTAL_PAISES,"wt");
    if(!pf)
        return;

    vectorOrdenar(infoT,Cmp_TCod,1);
    tTransferencia* vecTransferencia;

    int i=0;
    int fin=vectorGetCantElem(infoT);



    Vector* s_pais=vectorCrear(CANT_PAISES,sizeof(tSaldoPais));
    tSaldoPais aux;
    tSaldoPais* pais_aux;

    int cantRegistros,j;
    float debito,credito;
    char codActual[TAM_COD];
    char descActual[TAM_DESC_PAIS];



    while(i<fin)
    {
        vecTransferencia=vectorGetElementoPorPos(infoT,i);
        cantRegistros=0;
        debito=0;
        credito=0;
        strcpy(codActual,vecTransferencia->cod_pais);
        strcpy(descActual,vecTransferencia->desc_pais);
        while(i < fin && (strcmpi(codActual, vecTransferencia->cod_pais) == 0))
        {
            vecTransferencia=vectorGetElementoPorPos(infoT,i);
            cantRegistros++;

            if(vecTransferencia->operacion == DEBITO)
            debito += vecTransferencia->monto;
            else
            credito += vecTransferencia->monto;

            i++;
        }
        aux.cantRegistros=cantRegistros;
        strcpy(aux.cod_pais,codActual);
        strcpy(aux.desc_pais,descActual);
        aux.credito=credito;
        aux.debito=debito;

        vectorInsertarFinal(s_pais,&aux);

    }
    int finPais=vectorGetCantElem(s_pais);


    vectorOrdenar(s_pais,cmp_CreditoPais,2);

    fprintf(pf,"PAIS_COD;PAIS_DESC;registros;total_credito;total_debito\n");
    for(j=0;j<finPais;j++)
    {
        pais_aux=vectorGetElementoPorPos(s_pais,j);
        fprintf(pf,"%s;%s;%d;%.2f;%.2f\n",pais_aux->cod_pais,pais_aux->desc_pais,
        pais_aux->cantRegistros,pais_aux->credito,pais_aux->debito);

    }
    CerrarArch(pf);
    vectorDestruir(s_pais);



}
void GenerarArch_SaldoPorContinente(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_SALDO_CONTINENTE,"wt");
    FILE* pPais=AbrirArch(ARCH_PAISES,"rt");
    if(!pf || !pPais)
    {
        CerrarArch(pf);
        CerrarArch(pPais);
        return;
    }

    tTransferencia* vec;
    tPais Pais_aux;
    Vector* s_cont=vectorCrear(7,sizeof(tSaldoContinente));
    char linea[TAM_LINEA];
    int i;
    int j;
    int ceInfoT=vectorGetCantElem(infoT);

    InicializarVectorSaldoContinente(s_cont);

    for(j=0;j<ceInfoT;j++)
    {
        vec=vectorGetElementoPorPos(infoT,j);
        rewind(pPais);
        fgets(linea,TAM_LINEA,pPais);
        while(fgets(linea,TAM_LINEA,pPais))
        {
            trozarCampoVariable_Pais(&Pais_aux,linea);
            if(strcmpi(Pais_aux.cod_pais,vec->cod_pais)==0)
                AcumularSaldosSegunContinente(s_cont,&Pais_aux,vec);
        }
    }

    vectorOrdenar(s_cont,CmpCredito_Continente,2);
    fprintf(pf,"CONTINENTE;Registros;Total_Credito;Total_Debito\n");

    for(i=0;i<vectorGetCantElem(s_cont);i++)
    {
        tSaldoContinente* saldo = vectorGetElementoPorPos(s_cont,i);
        fprintf(pf,"%s;%d;%.2f;%.2f\n", saldo->continente, saldo->cantRegistros,
                                        saldo->credito, saldo->debito);
    }
    CerrarArch(pf);
    CerrarArch(pPais);
    vectorDestruir(s_cont);
}
void InicializarVectorSaldoContinente(Vector* vec)
{
    FILE* pPaises=AbrirArch(ARCH_PAISES,"rt");
    if(!pPaises)
        return;

    char linea[TAM_LINEA];
    tPais infoP;
    tSaldoContinente nuevo;
    int i;
    bool existe;

    fgets(linea,TAM_LINEA,pPaises);

    while(fgets(linea,TAM_LINEA,pPaises))
    {
        trozarCampoVariable_Pais(&infoP,linea);
        existe = false;

        for(i=0; i < vectorGetCantElem(vec); i++)
        {
            tSaldoContinente* saldo = vectorGetElementoPorPos(vec, i);
            if(strcmpi(saldo->continente, infoP.continente) == 0)
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
            vectorInsertarFinal(vec, &nuevo);
        }
    }
    CerrarArch(pPaises);
}
void AcumularSaldosSegunContinente(Vector* vecSaldoContinente, tPais* infoPais,tTransferencia* vecTransferencia)
{
    int i;
    for(i=0; i < vectorGetCantElem(vecSaldoContinente); i++)
    {
        tSaldoContinente* saldo = vectorGetElementoPorPos(vecSaldoContinente, i);
        if(strcmpi(infoPais->continente, saldo->continente) == 0)
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
void GenerarArch_SaldoTrimestre(Vector* infoT)
{
    FILE* pf = AbrirArch(ARCH_SALDO_TRIMESTRE, "wt");
    if(!pf)
        return;

    int anio;
    int i=0;
    int ce=vectorGetCantElem(infoT);

    bool tieneOperacion;

    char paisActual[TAM_DESC_PAIS];
    char codActual[TAM_COD];

    float vecSaldoTrimestre[TAM_TRIMESTRE];

    tTransferencia* vec;
    


    //vec = (tTransferencia*)infoT->vec;
    //fin = vec + infoT->ce;
    PonerEnCeroVecFloat(vecSaldoTrimestre);


    printf("\nIngrese un anio para mostrar en la tabla (2015-Actualidad): ");
    anio = ValidarRango_Int(ANIO_MENOR, ANIO_ACTUAL);
    vectorOrdenar(infoT, Cmp_TDesc, 1);

    fprintf(pf, "%-40s %8s %8s %8s %8s\n","Pais", "T1", "T2", "T3", "T4");
    while(i<ce)
    {
        
        strcpy(codActual, vec->cod_pais);
        strcpy(paisActual, vec->desc_pais);
        tieneOperacion = false;

        while(i<ce && strcmpi(paisActual, vec->desc_pais) == 0)
        {
            vec=vectorGetElementoPorPos(infoT,i);
            if(vec->anio == anio)
                AcumularSaldoTrimestre(vec,vecSaldoTrimestre,&tieneOperacion);
            i++;
        }

        if(tieneOperacion)
        {
            fprintf(pf,"%-40s %8.2f %8.2f %8.2f %8.2f\n", paisActual,
                    vecSaldoTrimestre[0], vecSaldoTrimestre[1],
                    vecSaldoTrimestre[2], vecSaldoTrimestre[3]);
        }
    }

    CerrarArch(pf);
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
    return strcmpi(x->cod_pais,y->cod_pais);
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
    scanf("%d",&dato);
    while(dato<LimI || dato>LimS)
    {
        printf("\nError - Ingrese un anio entre %d y %d",LimI, LimS);
        scanf("%d",&dato);
    }
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
void GenerarArch_SaldoAnio(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_SALDO_ANIO,"wt");

    if(!pf)
        return;

    vectorOrdenar(infoT,Cmp_TAnio,1);

    tTransferencia* vecT;
    //tTransferencia* pfin=vecT+infoT->ce;

    float total_debito, total_credito, variacion,saldoActual, saldoAnterior=0;

    int anioActual;
    int i=0;
    int ce=vectorGetCantElem(infoT);

    fprintf(pf,"ANIO;C;D;SALDO;VARIACION\n");

    while(i<ce)
    {
        anioActual=vecT->anio;
        total_credito=0;
        total_debito=0;
        saldoActual=0;
        variacion=0;

        while(i<ce && anioActual==vecT->anio)
        {
            vecT=vectorGetElementoPorPos(infoT,i);
            if(vecT->operacion==CREDITO)
            {
                total_credito+=vecT->monto;
            }
            else
            {
                total_debito+=vecT->monto;
            }
            i++;
        }

        saldoActual=total_credito-total_debito;

        if(anioActual==ANIO_MENOR)
        {
            fprintf(pf,"%d;%.2f;%.2f;%.2f;%s\n",anioActual,total_credito,total_debito,saldoActual,SIN_VARIACION);
        }
        else
        {
            variacion= ((saldoActual-saldoAnterior)/saldoAnterior)*100;
            fprintf(pf,"%d;%.2f;%.2f;%.2f;%.2f\n",anioActual,total_credito,total_debito,saldoActual,variacion);
        }
        saldoAnterior=saldoActual;
    }
    CerrarArch(pf);

}
void GenerarArch_SaldoPais_SegunOperacion(Vector* infoT)
{
    FILE* pf= AbrirArch(ARCH_SALDO_PAISES_OPERACION,"wt");
    if(!pf)
        return;

    vectorOrdenar(infoT,Cmp_TDesc,MENOR_A_MAYOR);

    Vector* infoP=vectorCrear(CANT_PAISES,sizeof(tSaldoPais));

    tSaldoPais* vecP;
    tSaldoPais actual;

    tTransferencia* vecT;
    

    int cantPaisesListar,i,j=0;
    int ceTransferencia=vectorGetCantElem(infoT);
    char opcionOperacion;

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

        while(j<ceTransferencia && strcmpi(actual.desc_pais,vecT->desc_pais)==0)
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
        vectorInsertarFinal(infoP,&actual);


    }
    fprintf(pf,"COD_PAIS;DESC_PAIS;Registros;Total_credito;Total_debito\n");

    switch(opcionOperacion)
    {
        case CREDITO:
                vectorOrdenar(infoP,Cmp_SPCredito,MAYOR_A_MENOR);
                break;
        case DEBITO:
                vectorOrdenar(infoP,Cmp_SPDebito,MAYOR_A_MENOR);
                break;
    }
    for(i=0;i<cantPaisesListar;i++)
    {
        vecP=vectorGetElementoPorPos(infoP,i);
        fprintf(pf,"%s;%s,%d;%.2f;%.2f\n",vecP->cod_pais,vecP->desc_pais,vecP->cantRegistros,
                                            vecP->credito,vecP->debito);
        
    }
    CerrarArch(pf);

}
void GenerarArch_SaldoContinentePorcentaje(Vector* infoT)
{
    FILE* pf=AbrirArch(ARCH_SALDO_CONTINENTE_PORCENTAJE,"w+b");
    FILE* pPais=AbrirArch(ARCH_PAISES,"rt");

    if(!pf || !pPais)
    {
        CerrarArch(pf);
        CerrarArch(pPais);
        return;
    }

    Vector* infoCPorcentaje=vectorCrear(CANT_CONTINENTE,sizeof(tPorcentajeContinente));
    Vector* s_cont= vectorCrear(CANT_CONTINENTE,sizeof(tSaldoContinente));

    tTransferencia* vecT;
   
    int i;
    int ceTransferencia=vectorGetCantElem(infoT);
    int j;

    tPais Pais_aux;

    tSaldoContinente* saldoContinente;

    tPorcentajeContinente PC_Aux;
    tPorcentajeContinente* pPC_Aux;

    float TotalDebito=0,TotalCredito=0;

    char linea[TAM_LINEA];

    InicializarVectorSaldoContinente(s_cont);

    CalcularTotalDeOperaciones(infoT,&TotalDebito,&TotalCredito);

    for(i=0;i<ceTransferencia;i++)
    {
        rewind(pPais);
        fgets(linea,TAM_LINEA,pPais);
        while(fgets(linea,TAM_LINEA,pPais))
        {
            vecT=vectorGetElementoPorPos(infoT,i);
            trozarCampoVariable_Pais(&Pais_aux,linea);
            if(strcmpi(Pais_aux.cod_pais,vecT->cod_pais)==0)
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

        vectorInsertarFinal(infoCPorcentaje,&PC_Aux);
        
    }

    vectorOrdenar(infoCPorcentaje,Cmp_Porc_Continente,MENOR_A_MAYOR);

    for(j=0;j<vectorGetCantElem(infoCPorcentaje);j++)
    {
        pPC_Aux=vectorGetElementoPorPos(infoCPorcentaje,j);
        fwrite(pPC_Aux,sizeof(tPorcentajeContinente),1,pf);
        
    }

    CerrarArch(pf);
    CerrarArch(pPais);

    LeerArchivo_PorcentajeContinente();

    vectorDestruir(infoCPorcentaje);
    vectorDestruir(s_cont);
}
void LeerArchivo_PorcentajeContinente()
{
    FILE* pf=AbrirArch(ARCH_SALDO_CONTINENTE_PORCENTAJE,"rb");
    char opcionParaLeer;

    tPorcentajeContinente PC_Aux;

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
    CerrarArch(pf);

}
void GenerarArch_BaseLarga(Vector* infoT)
{
    FILE* pf= AbrirArch(ARCH_BASE_LARGA,"wt");
    FILE* pPais=AbrirArch(ARCH_PAISES,"rt");

    if(!pf || !pPais)
        return;

    vectorOrdenar(infoT,Cmp_Transferencia_VariosCampos,MENOR_A_MAYOR);

    tTransferencia* vecT;
    tTransferencia auxT;

    int i=0;
    int ceTransferencia=vectorGetCantElem(infoT);

    char continente[TAM_CONTINENTE];

    fprintf(pf,"ANIO;TRIMESTRE;PAIS_COD;PAIS_DEC;CONTINENTE;OPERACION;MONTO\n");
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
                    && auxT.operacion==vecT->operacion  && strcmpi(auxT.cod_pais,vecT->cod_pais)==0)
        {
            auxT.monto+=vecT->monto;
            vecT=vectorGetElementoPorPos(infoT,++i);
        }
        EncontrarContinente_SegunPais(continente,&auxT,pPais);
        fprintf(pf,"%d;%d;%s;%s;%s;%c;%.2f\n",auxT.anio,auxT.trimestre,auxT.cod_pais,auxT.desc_pais,continente,auxT.operacion,auxT.monto);
    }
    CerrarArch(pf);
    CerrarArch(pPais);

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
        if(strcmpi(vecP.cod_pais,auxT->cod_pais)==0)
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
    return strcmpi(x->desc_pais, y->desc_pais);
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
    return x->debito - y->debito;
}
int Cmp_SPCredito(void* a, void* b)
{
    tSaldoPais* x=(tSaldoPais*)a;
    tSaldoPais* y=(tSaldoPais*)b;
    return x->credito - y->credito;
}
int Cmp_Porc_Continente(void* a, void* b)
{
    tPorcentajeContinente* x = (tPorcentajeContinente*)a;
    tPorcentajeContinente* y = (tPorcentajeContinente*)b;
    return strcmpi(x->continente,y->continente);
}

int Cmp_Transferencia_VariosCampos(void* a, void* b)
{
    tTransferencia* t1 = (tTransferencia*)a;
    tTransferencia* t2 = (tTransferencia*)b;

    if(t1->anio != t2->anio)
        return t1->anio - t2->anio;

    if(t1->trimestre != t2->trimestre)
        return t1->trimestre - t2->trimestre;

    if(strcmpi(t1->cod_pais,t2->cod_pais)!=0)
        return t1->cod_pais - t2->cod_pais;

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

void CargarInfo_BaseLarga(Vector* infoBS, FILE* pf)
{
    char linea[TAM_LINEA];
    tBase_Larga aux;

    fgets(linea,TAM_LINEA,pf);

    while(fgets(linea,TAM_LINEA,pf))
    {
        trozarCampoVariable_BaseLarga(&aux,linea);
        vectorInsertarFinal(infoBS,&aux);
    }
}
void GenerarArch_Promedio_Pais_Trimestre(Vector* infoBL)
{
    FILE* pf=AbrirArch(ARCH_PROMEDIO_PAIS_TRIMESTRE,"wt");
    if(!pf)
        return;

    vectorOrdenar(infoBL,Cmp_BaseLarga_Pais_Operacion,MENOR_A_MAYOR);

    Vector* infoPPT=vectorCrear(TAM_VEC_MIN,sizeof(tPromedio_Pais_Trimestre));
    tPromedio_Pais_Trimestre auxPPT;
    tPromedio_Pais_Trimestre* pAux;

    tBase_Larga* vecBS= (tBase_Larga*)infoBL->vec;
    tBase_Larga* pfin=vecBS+infoBL->ce;

    int i;

    fprintf(pf,"PAIS_DESC;OPERACION;Suma_monto;Cant_trimestre;Promedio\n");

    while(vecBS< pfin)
    {
        strcpy(auxPPT.desc_pais,vecBS->desc_pais);
        auxPPT.operacion=vecBS->operacion;
        auxPPT.cant_trimestre=0;
        auxPPT.promedio_credito=0;
        auxPPT.promedio_debito=0;
        auxPPT.sumaMontos=0;
        while(vecBS<pfin && strcmpi(auxPPT.desc_pais,vecBS->desc_pais)==0 && auxPPT.operacion==vecBS->operacion)
        {
            if(vecBS->monto>0)
                auxPPT.cant_trimestre++;

            auxPPT.sumaMontos+=vecBS->monto;
            vecBS++;
        }
        if(auxPPT.cant_trimestre!=0)
        {
            if(auxPPT.operacion==CREDITO)
                auxPPT.promedio_credito=auxPPT.sumaMontos/auxPPT.cant_trimestre;
            else
                auxPPT.promedio_debito=auxPPT.sumaMontos/auxPPT.cant_trimestre;
        }
        vectorInsertarFinal(infoPPT,&auxPPT);
    }
    vectorOrdenar(infoPPT,Cmp_PPT_DescPais,MENOR_A_MAYOR);
    pAux=(tPromedio_Pais_Trimestre*)infoPPT->vec;

    for(i=0;i<infoPPT->ce;i++)
    {
        if(pAux->operacion==CREDITO)
            fprintf(pf,"%s;%c;%.2f;%d;%.2f\n",pAux->desc_pais,pAux->operacion,pAux->sumaMontos,pAux->cant_trimestre,pAux->promedio_credito);
        else
            fprintf(pf,"%s;%c;%.2f;%d;%.2f\n",pAux->desc_pais,pAux->operacion,pAux->sumaMontos,pAux->cant_trimestre,pAux->promedio_debito);
        pAux++;
    }
    CerrarArch(pf);
    vectorDestruir(infoPPT);

}
void GenerarArch_Promedio_Anio_Continente_Op(Vector* infoBL)
{
    FILE* pf=AbrirArch(ARCH_PROMEDIO_CONTINENTE_ANI_OP,"wt");
    if(!pf)
        return;

    vectorOrdenar(infoBL,Cmp_Prom_ACO,MENOR_A_MAYOR);

    tBase_Larga* vecBL = (tBase_Larga*)infoBL->vec;
    tBase_Larga* pfin = vecBL+infoBL->ce;
    tBase_Larga aux;

    int cant_registros;
    float promedio;

    fprintf(pf,"ANIO;CONTINENTE;OPERACION;Suma_monto;cant_trimestre;PROMEDIO\n");

    while(vecBL<pfin)
    {
        aux.anio=vecBL->anio;
        strcpy(aux.continente,vecBL->continente);
        aux.operacion=vecBL->operacion;
        aux.monto=0;
        cant_registros=0;
        promedio=0;

        while(vecBL<pfin && aux.anio==vecBL->anio && aux.operacion==vecBL->operacion && strcmpi(aux.continente,vecBL->continente)==0)
        {
            aux.monto+=vecBL->monto;
            if(vecBL->monto>0)
                cant_registros++;
            vecBL++;
        }
        if(cant_registros!=0)
            promedio=aux.monto/cant_registros;

        fprintf(pf,"%d;%s;%c;%.2f;%d;%.2f\n",aux.anio,aux.continente,aux.operacion,aux.monto,cant_registros,promedio);
    }
    CerrarArch(pf);

}


int Cmp_BaseLarga_Pais_Operacion(void* a, void* b)
{
    tBase_Larga* x = (tBase_Larga*)a;
    tBase_Larga* y = (tBase_Larga*)b;

    if(strcmpi(x->desc_pais,y->desc_pais)!=0)
        return strcmpi(x->desc_pais,y->desc_pais);

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
    return strcmpi(x->desc_pais,y->desc_pais);
}
int Cmp_Prom_ACO(void* a, void* b)
{
    tBase_Larga* x = (tBase_Larga*)a;
    tBase_Larga* y = (tBase_Larga*)b;


    if(x->anio!=y->anio)
        return x->anio - y->anio;

    if(strcmpi(x->continente,y->continente)!=0)
        return strcmpi(x->continente,y->continente);

    if(x->operacion!=y->operacion)
            return x->operacion - y->operacion;

    return 0;

}



