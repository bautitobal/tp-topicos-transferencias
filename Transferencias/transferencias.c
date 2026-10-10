#include "transferencias.h"


void trozarCampoVariable_Pais(FILE* pPais,tPais* info,char* linea)
{

    char* aux;

    //fgets(linea,TAM_LINEA,pPais);
    fgets(linea,TAM_LINEA,pPais);

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
    //fgets(linea,TAM_LINEA,pTransferencia);
    //fgets(linea,TAM_LINEA,pTransferencia);

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
    
    
    vectorOrdenar(s_pais,cmp_Credito,2);
    
    fprintf(pf,"PAIS_COD;PAIS_DESC;registros;total_credito;total_debito\n");
    for(i=0;i<s_pais->ce;i++)
    {
        fprintf(pf,"%s;%s;%d;%.2f;%.2f\n",pais_aux->cod_pais,pais_aux->desc_pais,
            pais_aux->cantRegistros,pais_aux->credito,pais_aux->debito);
            pais_aux++;
        }
        CerrarArch(pf);
        
        
        
    }
    int cmp_Credito(void* a, void* b)
    {
        tSaldoPais* x= (tSaldoPais*)a;
        tSaldoPais* y= (tSaldoPais*)b;
        return (x->credito > y->credito) - (x->credito < y->credito);
    }
    int Cmp_T(void* a, void* b)
    {
        tTransferencia* x= (tTransferencia*)a;
        tTransferencia* y= (tTransferencia*)b;
        return strcmpi(x->cod_pais,y->cod_pais);
    }