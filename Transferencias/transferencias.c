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
    tTransferencia* vec=(tTransferencia*)infoT->vec;
    tTransferencia* pfin=vec+infoT->ce-1;

    Vector* s_pais=vectorCrear(CANT_PAISES,sizeof(tSaldoPais));
    tSaldoPais aux;
    tSaldoPais* pais_aux=(tSaldoPais*)s_pais->vec;

    int cantRegistros,i;
    float debito,credito;
    char codActual[TAM_COD];
    char descActual[TAM_DESC_PAIS];


    while(vec<=pfin)
    {
        cantRegistros=0;
        debito=0;
        credito=0;
        strcpy(codActual,vec->cod_pais);
        strcpy(descActual,vec->desc_pais);
        while(vec <= pfin && strcmpi(codActual, vec->cod_pais) == 0)
        {
            cantRegistros++;

            if(vec->operacion == DEBITO)
            debito += vec->monto;
            else
            credito += vec->monto;

            vec++;
        }
        aux.cantRegistros=cantRegistros;
        strcpy(aux.cod_pais,codActual);
        strcpy(aux.desc_pais,descActual);
        aux.credito=credito;
        aux.debito=debito;

        vectorInsertarFinal(s_pais,&aux);

    }


    vectorOrdenar(s_pais,cmp_CreditoPais,2);

    fprintf(pf,"PAIS_COD;PAIS_DESC;registros;total_credito;total_debito\n");
    for(i=0;i<s_pais->ce;i++)
    {
        fprintf(pf,"%s;%s;%d;%.2f;%.2f\n",pais_aux->cod_pais,pais_aux->desc_pais,
            pais_aux->cantRegistros,pais_aux->credito,pais_aux->debito);
            pais_aux++;
    }
    CerrarArch(pf);



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

    tTransferencia* vec=(tTransferencia*)infoT->vec;
    tTransferencia* pfin=vec+infoT->ce-1;
    tTransferencia* j;

    tPais Pais_aux;

    Vector* s_cont=vectorCrear(7,sizeof(tSaldoContinente));
    tSaldoContinente* auxContinente=(tSaldoContinente*)s_cont->vec;

    char linea[TAM_LINEA];

    int i;


    InicializarVectorSaldoContinente(s_cont);



    for(j=vec;j<=pfin;j++)
    {
        rewind(pPais);
        fgets(linea,TAM_LINEA,pPais);
        while(fgets(linea,TAM_LINEA,pPais))
        {
            trozarCampoVariable_Pais(&Pais_aux,linea);
            if(strcmpi(Pais_aux.cod_pais,j->cod_pais)==0)
                AcumularSaldosSegunContinente(s_cont,&Pais_aux,j);

        }
    }
    vectorOrdenar(s_cont,CmpCredito_Continente,2);
    fprintf(pf,"CONTINENTE;Registros;Total_Credito;Total_Debito\n");

    for(i=0;i<s_cont->ce;i++)
    {
        fprintf(pf,"%s;%d;%.2f;%.2f\n",auxContinente->continente,auxContinente->cantRegistros,
                                        auxContinente->credito,auxContinente->debito);
        auxContinente++;
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

    fgets(linea,TAM_LINEA,pPaises);

    while(fgets(linea,TAM_LINEA,pPaises))
    {
        trozarCampoVariable_Pais(&infoP,linea);
        tSaldoContinente* ini = (tSaldoContinente*)vec->vec;
        tSaldoContinente* fin = ini + vec->ce;
        tSaldoContinente* p = ini;

        while (p < fin && strcmpi(p->continente, infoP.continente) != 0)
            p++;

        if (p == fin)
        {
            strcpy(nuevo.continente, infoP.continente);
            nuevo.cantRegistros = 0;
            nuevo.credito = 0;
            nuevo.debito= 0;
            vectorInsertarFinal(vec, &nuevo);
        }
    }
    CerrarArch(pPaises);
    CerrarArch(pPaises);


}
void AcumularSaldosSegunContinente(Vector* vecSaldoContinente, tPais* infoPais,tTransferencia* vecTransferencia)
{
    tSaldoContinente* aux=(tSaldoContinente*)vecSaldoContinente->vec;
    tSaldoContinente* i;
    tSaldoContinente* pfin=aux+vecSaldoContinente->ce;
    for(i=aux;i<pfin;i++)
    {
        if(strcmpi(infoPais->continente,i->continente)==0)
        {
            i->cantRegistros++;
            if(vecTransferencia->operacion==CREDITO)
            {
                i->credito+=vecTransferencia->monto;
            }
            else
            {
                i->debito+=vecTransferencia->monto;
            }
        }
    }

}
void GenerarArch_SaldoTrimestre(Vector* infoT)
{
    FILE* pf = AbrirArch(ARCH_SALDO_TRIMESTRE, "wt");
    int anio;
    bool tieneOperacion;
    char paisActual[TAM_DESC_PAIS];
    char codActual[TAM_COD];
    float vecSaldoTrimestre[TAM_TRIMESTRE];
    tTransferencia* vec;
    tTransferencia* fin;

    if(!pf)
        return;

    vec = (tTransferencia*)infoT->vec;
    fin = vec + infoT->ce;

    printf("\nIngrese un anio para mostrar en la tabla (2015-Actualidad): ");
    anio = ValidarAnio(ANIO_MENOR, ANIO_ACTUAL);
    vectorOrdenar(infoT, Cmp_TDesc, 1);

    fprintf(pf, "Pais    T1  T2  T3  T4\n");
    while(vec < fin)
    {
        strcpy(codActual, vec->cod_pais);
        strcpy(paisActual, vec->desc_pais);
        PonerEnCeroVecFloat(vecSaldoTrimestre);
        tieneOperacion = false;

        while(vec < fin && strcmpi(paisActual, vec->desc_pais) == 0)
        {
            if(vec->anio == anio)
                AcumularSaldoTrimestre(vec,vecSaldoTrimestre,&tieneOperacion);
            vec++;
        }

        if(tieneOperacion)
        {
            fprintf(pf, "%s    %.2f %.2f %.2f %.2f\n", paisActual,
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
int ValidarAnio(int LimI, int LimS)
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
int Cmp_TDesc(void* a, void* b)
{
    tTransferencia* x = (tTransferencia*)a;
    tTransferencia* y = (tTransferencia*)b;
    return strcmpi(x->desc_pais, y->desc_pais);
}