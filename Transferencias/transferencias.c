#include "transferencias.h"

/* ======================================================================== */
/* FUNCIONES COMPARTIDAS                                                    */
/* Se usan desde más de un punto del programa. Encima de cada una se        */
/* aclara en qué opciones/funciones se utiliza. Las funciones que usa       */
/* una sola opción están más abajo, en la sección de esa opción.            */
/* ======================================================================== */

/* USADA EN VARIOS PUNTOS (compara textos sin distinguir mayúsculas):
   - opción 1: GenerarArch_SaldoPorPais y el comparador Cmp_TCod
   - opción 2: GenerarArch_SaldoPorContinente, InicializarVectorSaldoContinente
               y AcumularSaldosSegunContinente
   - opción 3: GenerarArch_SaldoTrimestre y el comparador Cmp_TDesc
   - opción 5: GenerarArch_SaldoPais_SegunOperacion y el comparador Cmp_TDesc
   - opción 6: GenerarArch_SaldoContinentePorcentaje, InicializarVectorSaldoContinente,
               AcumularSaldosSegunContinente y el comparador Cmp_Porc_Continente
   - opción 7: GenerarArch_BaseLarga, EncontrarContinente_SegunPais y el
               comparador Cmp_Transferencia_VariosCampos
   - opción 8: GenerarArch_Promedio_Pais_Trimestre y los comparadores
               Cmp_BaseLarga_Pais_Operacion y Cmp_PPT_DescPais
   - opción 9: GenerarArch_Promedio_Anio_Continente_Op y el comparador Cmp_Prom_ACO
   - main.c: Menu_Principal (para comprobar si la respuesta es "No") */
int compararSinMayusculas(const char* c1, const char* c2)
{
    /* compara dos cadenas ignorando diferencias entre mayúsculas y minúsculas. */
    while(*c1 && *c2 && tolower((unsigned char)*c1) == tolower((unsigned char)*c2))
    {
        c1++;
        c2++;
    }

    return tolower((unsigned char)*c1) - tolower((unsigned char)*c2);
}

/* USADA EN VARIOS PUNTOS (abre un archivo y avisa por pantalla si no puede):
   - opciones 1 a 9: todas las funciones GenerarArch_* abren con ella su archivo
     de salida (la opción 7 también abre paises_continentes.csv)
   - opciones 2 y 6: InicializarVectorSaldoContinente (abre paises_continentes.csv)
   - opción 6: LeerArchivo_PorcentajeContinente (abre el archivo .dat generado)
   - main.c: Menu_Principal (abre el CSV de transferencias) y
     CargarBaseLarga (abre la base larga) */
FILE* AbrirArch(const char* nombre, const char* modo)
{    
    /* centraliza la apertura de archivos y el mensaje de error. */
    FILE* pf=fopen(nombre, modo);

    if(!pf)
    {
        printf("\nError al abrir el archivo %s \n", nombre);
        return NULL;
    }
    
    return pf;
}

/* USADA EN VARIOS PUNTOS (pide un entero por teclado hasta que esté entre LimI y LimS):
   - main.c: Menu_Principal (opción del menú, de 1 a 12)
   - opción 3: GenerarArch_SaldoTrimestre (año a consultar)
   - opción 5: GenerarArch_SaldoPais_SegunOperacion (cantidad de países a listar)
   - opción 10: RankPorSaldoNeto (año y cantidad de países a mostrar) */
int ValidarRango_Int(int LimI, int LimS)
{
    /* lee un entero hasta recibir un valor dentro del rango solicitado. */
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

/* USADA EN VARIOS PUNTOS (pide un carácter por teclado hasta que sea uno de los dos permitidos):
   - opción 5: GenerarArch_SaldoPais_SegunOperacion (elegir crédito 'C' o débito 'D')
   - opción 6: LeerArchivo_PorcentajeContinente (responder 'S' o 'N' para leer el archivo) */
char ValidarChar(char a, char b)
{
    /* lee una opción de carácter hasta que coincida con una de las permitidas. */
    char dato;
    scanf(" %c",&dato);
    while(dato!= a && dato!=b)
    {
        printf("\nError - La opcion es %c o %c: ",a,b);
        scanf(" %c",&dato);
    }
    return dato;
}

/* USADA EN VARIOS PUNTOS (separa una línea de paises_continentes.csv):
   - opción 2: GenerarArch_SaldoPorContinente
   - opciones 2 y 6: InicializarVectorSaldoContinente
   - opción 6: GenerarArch_SaldoContinentePorcentaje
   - opción 7: EncontrarContinente_SegunPais */
bool trozarCampoVariable_Pais(tPais* info,char* linea)
{
    /* separa una línea de paises_continentes.csv y completa la estructura tPais.
       formato de la línea: PAIS_COD;PAIS_DESC;CONTINENTE

       la línea se recorre de derecha a izquierda: se busca el último ';' con
       strrchr, se copia lo que hay a su derecha y se lo reemplaza por '\0'. así
       la línea se va acortando hasta que sólo queda el código de país.
       OJO: modifica el contenido de 'linea'.

       devuelve true si pudo completar los tres campos, false si a la línea le
       faltan separadores (línea vacía o incompleta). */

    char* aux;

    /* la ultima linea del archivo puede no tener '\n' */
    aux=strchr(linea,'\n');
    if(aux)
        *aux='\0';

    /* último campo: CONTINENTE */
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    strcpy(info->continente,aux+1);

    /* campo del medio: PAIS_DESC */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    strcpy(info->desc_pais,aux+1);

    /* lo único que queda en la línea es el primer campo: PAIS_COD */
    *aux='\0';
    strcpy(info->cod_pais,linea);

    return true;
}

/* USADA EN VARIOS PUNTOS (arma el vector con un saldo en cero por cada continente):
   - opción 2: GenerarArch_SaldoPorContinente
   - opción 6: GenerarArch_SaldoContinentePorcentaje */
int InicializarVectorSaldoContinente(Vector* vec)
{
    /* carga una entrada inicial para cada continente presente en el archivo auxiliar. */
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
        /* se saltean las lineas vacias o incompletas */
        if(!trozarCampoVariable_Pais(&infoP,linea))
            continue;

        existe = false;

        for(i=0; i < (int)vectorGetCantElem(vec); i++)
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

            if(codRet != TODO_OK)
            {
                fclose(pPaises);
                return codRet;
            }
        }
    }
    fclose(pPaises);
    return TODO_OK;
}

/* USADA EN VARIOS PUNTOS (suma una transferencia al saldo de su continente):
   - opción 2: GenerarArch_SaldoPorContinente
   - opción 6: GenerarArch_SaldoContinentePorcentaje */
void AcumularSaldosSegunContinente(Vector* vecSaldoContinente, tPais* infoPais,tTransferencia* vecTransferencia)
{
    /* actualiza el continente del país con el importe de una transferencia. */
    int i;
    for(i=0; i < (int)vectorGetCantElem(vecSaldoContinente); i++)
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

/* USADA EN VARIOS PUNTOS (comparador para vectorOrdenar, ordena por nombre de país):
   - opción 3: GenerarArch_SaldoTrimestre
   - opción 5: GenerarArch_SaldoPais_SegunOperacion */
int Cmp_TDesc(void* a, void* b)
{
    /* ordena transferencias alfabéticamente por descripción de país. */
    tTransferencia* x = (tTransferencia*)a;
    tTransferencia* y = (tTransferencia*)b;
    return compararSinMayusculas(x->desc_pais, y->desc_pais);
}

/* ======================================================================== */
/* CARGA INICIAL DE LAS TRANSFERENCIAS                                      */
/* Se ejecuta una sola vez al iniciar el programa (Menu_Principal, main.c). */
/* ======================================================================== */

bool trozarCampoVariable_Transferencia(tTransferencia* info, char* linea)
{
    /* separa una línea de transferencias_personales_clean.csv y completa tTransferencia.
       formato de la línea: ANIO;TRIMESTRE;PAIS_COD;PAIS_DESC;OPERACION;MONTO

       usa la misma técnica que trozarCampoVariable_Pais: va de derecha a izquierda,
       copia/convierte lo que hay después del último ';' y lo corta con '\0'.
       los campos numéricos se convierten con sscanf (%f monto, %d trimestre y año)
       y la operación se lee como un carácter ('C' crédito, 'D' débito).
       OJO: modifica el contenido de 'linea'. no se verifica el valor devuelto
       por sscanf, o sea que no se valida que los números sean correctos.

       devuelve true si la línea tenía todos los separadores, false si no. */
    char* aux;
    tTransferencia* infoT=(tTransferencia*)info;


    /* la ultima linea del archivo puede no tener '\n' */
    aux=strchr(linea,'\n');
    if(aux)
        *aux='\0';

    /* 6to campo (último): MONTO */
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    sscanf(aux+1,"%f",&infoT->monto);

    /* 5to campo: OPERACION */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    sscanf(aux+1,"%c",&infoT->operacion);

    /* 4to campo: PAIS_DESC */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    strcpy(infoT->desc_pais,aux+1);

    /* 3er campo: PAIS_COD */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    strcpy(infoT->cod_pais,aux+1);

    /* 2do campo: TRIMESTRE */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    sscanf(aux+1,"%d",&infoT->trimestre);

    /* lo único que queda en la línea es el primer campo: ANIO */
    *aux='\0';
    sscanf(linea,"%d",&infoT->anio);

    return true;
}

int CargarInfoTransferencia(Vector* infoT, FILE* pTransferencia)
{
    /* salta el encabezado, valida cada registro y lo agrega al vector. */
    tTransferencia aux;  
    char linea_T[TAM_LINEA]; /*El registro actual que lee*/
    int codRet;

    fgets(linea_T,TAM_LINEA,pTransferencia); /*Lee el encabezado (titulos) pero no lo usa para nada*/

    while(fgets(linea_T,TAM_LINEA,pTransferencia))
    {
        /* se saltean las lineas vacias o incompletas */
        if(!trozarCampoVariable_Transferencia(&aux, linea_T)) /*Llena la estructura con sus campos*/
            continue;

        codRet = vectorInsertarFinal(infoT,&aux); 

        if (codRet != TODO_OK) /*En caso de error devuelve el motivo*/
            return codRet;
    }

    codRet = TODO_OK;
    return codRet;
}

/* ======================================================================== */
/* OPCIÓN 1 - Totales por país                                              */
/* ======================================================================== */

int GenerarArch_SaldoPorPais(Vector* infoT)
{
    /* agrupa las transferencias por país y genera sus totales de crédito y débito. */
    FILE* pf=AbrirArch(ARCH_TRANSTOTAL_PAISES,"wt");

    tTransferencia* vecTransferencia; /*Crea un puntero a una estructura vecTransferencia*/

    int i=0;
    int fin=vectorGetCantElem(infoT); /*Verifica la cantidad de elementos para apuntar al final*/
    int finPais;
    int codRet;

    Vector* s_pais; 

    tSaldoPais aux;
    tSaldoPais* pais_aux;

    int cantRegistros,j;
    float debito,credito; /* credito: dinero que ingresa a argentina, débito: dinero que sale de argentina hacia otro país */
    char codActual[TAM_COD];
    char descActual[TAM_DESC_PAIS];

    if(!pf) /*En caso de no poder abrirlo*/
        return ERROR_ARCH;

    vectorOrdenar(infoT,Cmp_TCod,1);

    s_pais = vectorCrear(CANT_PAISES,sizeof(tSaldoPais)); /*Crea un vector de paises*/
    
    if (s_pais == NULL) /*Verifica que haya espacio para el vector*/
    {
        fclose(pf);
        return SIN_MEM;
    }

    vecTransferencia=vectorGetElementoPorPos(infoT,i); /*Busca el registro de la pos=i*/

    while(i<fin) /*Controla la cantidad de registros del archivo original*/
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

        if (codRet != TODO_OK) /*Prueba de fallos*/
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

int Cmp_TCod(void* a, void* b)
{
    /* ordena transferencias por código de país. */
    tTransferencia* x= (tTransferencia*)a;
    tTransferencia* y= (tTransferencia*)b;
    return compararSinMayusculas(x->cod_pais,y->cod_pais);
}

int cmp_CreditoPais(void* a, void* b)
{
    /* compara dos saldos de país por el total de crédito. */
    tSaldoPais* x= (tSaldoPais*)a;
    tSaldoPais* y= (tSaldoPais*)b;
    return (x->credito > y->credito) - (x->credito < y->credito);
}

/* ======================================================================== */
/* OPCIÓN 2 - Totales por continente                                        */
/* ======================================================================== */

int GenerarArch_SaldoPorContinente(Vector* infoT)
{
    /* relaciona cada transferencia con su país y acumula los totales por continente. */
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
            if(trozarCampoVariable_Pais(&Pais_aux,linea) &&
               compararSinMayusculas(Pais_aux.cod_pais,vec->cod_pais)==0)
                AcumularSaldosSegunContinente(s_cont,&Pais_aux,vec);
        }
    }

    vectorOrdenar(s_cont,CmpCredito_Continente,2);
    fprintf(pf,"CONTINENTE;Registros;Total_Credito;Total_Debito\n");
    printf("\n%-16s %10s %14s %14s\n","CONTINENTE","Registros","Total_Credito","Total_Debito");

    for(i=0;i<(int)vectorGetCantElem(s_cont);i++)
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

int CmpCredito_Continente(void* a, void* b)
{
    /* compara dos saldos de continente por el total de crédito. */
    tSaldoContinente* x=(tSaldoContinente*)a;
    tSaldoContinente* y=(tSaldoContinente*)b;
    return (x->credito > y->credito) - (x->credito < y->credito);
}

/* ======================================================================== */
/* OPCIÓN 3 - Saldo trimestral por país                                     */
/* ======================================================================== */

int GenerarArch_SaldoTrimestre(Vector* infoT)
{
    /* calcula el saldo de cada país separado por trimestre para un año elegido. */
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

void PonerEnCeroVecFloat(float* vec)
{
    /* reinicia los cuatro acumuladores trimestrales. */
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
    /* suma créditos y resta débitos en el acumulador del trimestre indicado. */
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

/* ======================================================================== */
/* OPCIÓN 4 - Totales anuales y variación interanual                        */
/* ======================================================================== */

static float RedondearDosDecimales(float valor)
{
    /* redondea un importe a dos decimales evitando truncar valores negativos. */
    int centesimos;

    if(valor >= 0)
        centesimos=(int)(valor*100+0.5f);
    else
        centesimos=(int)(valor*100-0.5f);

    return (float)centesimos/100;
}

int GenerarArch_SaldoAnio(Vector* infoT)
{
    /* agrupa por año, calcula el saldo anual y su variación interanual. */
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

    fprintf(pf,"ANIO;C;D;SALDO;VAR_PCT_SALDO\n");
    printf("\n%-6s %12s %12s %12s %12s\n","ANIO","C","D","SALDO","VAR_PCT_SALDO");

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

        saldoActual=RedondearDosDecimales(total_credito-total_debito);

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

int Cmp_TAnio(void* a, void* b)
{
    /* ordena transferencias cronológicamente por año. */
    tTransferencia* x= (tTransferencia*)a;
    tTransferencia* y= (tTransferencia*)b;
    return x->anio - y->anio;
}

/* ======================================================================== */
/* OPCIÓN 5 - Ranking de países por crédito o débito                        */
/* ======================================================================== */

int GenerarArch_SaldoPais_SegunOperacion(Vector* infoT)
{
    /* construye un ranking de países según crédito o débito, elegido por el usuario. */
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

int Cmp_SPCredito(void* a, void* b)
{
    /* compara saldos de país por el total de crédito. */
    tSaldoPais* x=(tSaldoPais*)a;
    tSaldoPais* y=(tSaldoPais*)b;
    return (x->credito > y->credito) - (x->credito < y->credito);
}

int Cmp_SPDebito(void* a, void* b)
{
    /* compara saldos de país por el total de débito. */
    tSaldoPais* x=(tSaldoPais*)a;
    tSaldoPais* y=(tSaldoPais*)b;
    return (x->debito > y->debito) - (x->debito < y->debito);
}

/* ======================================================================== */
/* OPCIÓN 6 - Participación porcentual por continente                       */
/* ======================================================================== */

int GenerarArch_SaldoContinentePorcentaje(Vector* infoT)
{
    /* calcula la participación porcentual de cada continente y la guarda en binario. */
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
            if(trozarCampoVariable_Pais(&Pais_aux,linea) &&
               compararSinMayusculas(Pais_aux.cod_pais,vecT->cod_pais)==0)
                AcumularSaldosSegunContinente(s_cont,&Pais_aux,vecT);

        }
    }

    for(j=0;j<(int)vectorGetCantElem(s_cont);j++)
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

    for(j=0;j<(int)vectorGetCantElem(infoCPorcentaje);j++)
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

void CalcularTotalDeOperaciones(Vector* vecT, float* totalDebito, float* totalCredito)
{
    /* acumula por separado todos los importes de crédito y débito. */
    int i;
    tTransferencia* aux;
    for(i=0;i<(int)vectorGetCantElem(vecT);i++)
    {
        aux=vectorGetElementoPorPos(vecT,i);

        if(aux->operacion==CREDITO)
            *totalCredito+=aux->monto;
        else
            *totalDebito+=aux->monto;

        
    }

}

int Cmp_Porc_Continente(void* a, void* b)
{
    /* ordena porcentajes de continente por nombre. */
    tPorcentajeContinente* x = (tPorcentajeContinente*)a;
    tPorcentajeContinente* y = (tPorcentajeContinente*)b;
    return compararSinMayusculas(x->continente,y->continente);
}

int LeerArchivo_PorcentajeContinente()
{
    /* pregunta si se desea leer y mostrar el archivo binario generado. */
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

/* ======================================================================== */
/* OPCIÓN 7 - Generación de la base larga                                   */
/* ======================================================================== */

int GenerarArch_BaseLarga(Vector* infoT)
{
    /* genera transferencias_base_larga.csv (opción 7 del menú).

       a partir de las transferencias originales (infoT) arma una fila por cada
       combinación de AÑO + TRIMESTRE + PAÍS + OPERACIÓN, con el MONTO sumado, y le
       agrega el CONTINENTE del país (que se busca en paises_continentes.csv).
       formato de salida: ANIO;TRIMESTRE;PAIS_COD;PAIS_DESC;CONTINENTE;OPERACION;MONTO

       pasos:
       1. ordena infoT por año, trimestre, país y operación, para que los registros
          de una misma combinación queden uno a continuación del otro.
          OJO: ordena el vector original en el lugar.
       2. recorre el vector agrupando registros consecutivos con la misma clave
          y acumulando su monto (si la clave no se repite, el monto queda igual).
       3. por cada grupo busca el continente y escribe la línea en el archivo.

       esta base es la entrada de las opciones 8 y 9 (promedios), que la leen
       con CargarInfo_BaseLarga.

       devuelve TODO_OK, o ERROR_ARCH si no pudo abrir alguno de los dos archivos. */
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

    /* ordena por año, trimestre, código de país y operación */
    vectorOrdenar(infoT,Cmp_Transferencia_VariosCampos,MENOR_A_MAYOR);

    fprintf(pBaseLarga,"ANIO;TRIMESTRE;PAIS_COD;PAIS_DESC;CONTINENTE;OPERACION;MONTO\n");
    vecT=vectorGetElementoPorPos(infoT,i);

    /* cada vuelta de este while procesa un grupo (una clave distinta) */
    while(i<ceTransferencia)
    {
        /* auxT guarda la clave del grupo y acumula el monto */
        auxT.anio=vecT->anio;
        strcpy(auxT.cod_pais,vecT->cod_pais);
        auxT.trimestre=vecT->trimestre;
        auxT.operacion=vecT->operacion;
        strcpy(auxT.desc_pais,vecT->desc_pais);
        auxT.monto=0;

        /* mientras el registro tenga la misma clave que el grupo, suma su monto
           y avanza. al salir, vecT apunta al primer registro del grupo siguiente */
        while(i<ceTransferencia &&  auxT.anio==vecT->anio && auxT.trimestre==vecT->trimestre
                    && auxT.operacion==vecT->operacion  && compararSinMayusculas(auxT.cod_pais,vecT->cod_pais)==0)
        {
            auxT.monto+=vecT->monto;
            vecT=vectorGetElementoPorPos(infoT,++i);
        }
        /* grupo terminado: busca su continente y escribe la fila */
        EncontrarContinente_SegunPais(continente,&auxT,pPais);
        fprintf(pBaseLarga,"%d;%d;%s;%s;%s;%c;%.2f\n",auxT.anio,auxT.trimestre,auxT.cod_pais,auxT.desc_pais,continente,auxT.operacion,auxT.monto);
    }
    fclose(pBaseLarga);
    fclose(pPais);

    return TODO_OK;
}

int Cmp_Transferencia_VariosCampos(void* a, void* b)
{
    /* ordena transferencias por año, trimestre, país y operación. */
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

void EncontrarContinente_SegunPais(char* continente, tTransferencia* auxT, FILE* pPais)
{
    /* busca en paises_continentes.csv el continente del país de 'auxT' (por su
       PAIS_COD, sin distinguir mayúsculas) y lo copia en 'continente'.

       'pPais' tiene que estar abierto en modo lectura. se vuelve al principio del
       archivo con rewind en cada llamada y se lee línea por línea hasta encontrar
       el código; no hay que cargar el archivo completo en memoria.
       OJO: si el código no figura en el archivo, 'continente' no se modifica
       (queda con el valor que tenía antes de llamar a la función). */
    bool encontrado=false;
    tPais vecP;
    char linea[TAM_LINEA];

    /* vuelve al inicio y saltea la línea de encabezado */
    rewind(pPais);
    fgets(linea,TAM_LINEA,pPais);

    while(!encontrado && fgets(linea,TAM_LINEA,pPais) )
    {
        if(trozarCampoVariable_Pais(&vecP,linea) &&
           compararSinMayusculas(vecP.cod_pais,auxT->cod_pais)==0)
        {
            strcpy(continente,vecP.continente);
            encontrado=true;
        }
    }


}

/* ======================================================================== */
/* CARGA DE LA BASE LARGA                                                   */
/* La usan las opciones 8 y 9: CargarBaseLarga (main.c) la carga la         */
/* primera vez que se elige una de las dos.                                 */
/* ======================================================================== */

bool trozarCampoVariable_BaseLarga(tBase_Larga* infoBL,char* linea)
{
    /* separa una línea de transferencias_base_larga.csv y completa tBase_Larga.
       formato de la línea: ANIO;TRIMESTRE;PAIS_COD;PAIS_DESC;CONTINENTE;OPERACION;MONTO

       es igual a trozarCampoVariable_Transferencia pero con un campo más
       (CONTINENTE, entre PAIS_DESC y OPERACION). va de derecha a izquierda:
       copia/convierte lo que hay después del último ';' y lo corta con '\0'.
       OJO: modifica el contenido de 'linea'. no se valida el contenido de los
       campos numéricos.

       devuelve true si la línea tenía todos los separadores, false si no. */
    char* aux;

    /* la ultima linea del archivo puede no tener '\n' */
    aux=strchr(linea,'\n');
    if(aux)
        *aux='\0';

    /* 7mo campo (último): MONTO */
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    sscanf(aux+1,"%f",&infoBL->monto);

    /* 6to campo: OPERACION */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    sscanf(aux+1,"%c",&infoBL->operacion);

    /* 5to campo: CONTINENTE */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    strcpy(infoBL->continente,aux+1);

    /* 4to campo: PAIS_DESC */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    strcpy(infoBL->desc_pais,aux+1);

    /* 3er campo: PAIS_COD */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    strcpy(infoBL->cod_pais,aux+1);

    /* 2do campo: TRIMESTRE */
    *aux='\0';
    aux=strrchr(linea,';');
    if(!aux)
        return false;
    sscanf(aux+1,"%d",&infoBL->trimestre);

    /* lo único que queda en la línea es el primer campo: ANIO */
    *aux='\0';
    sscanf(linea,"%d",&infoBL->anio);

    return true;
}

int CargarInfo_BaseLarga(Vector* infoBS, FILE* pf)
{
    /* carga en el vector 'infoBS' todos los registros de transferencias_base_larga.csv
       (el archivo que genera la opción 7 del menú).

       'pf' tiene que estar abierto en modo lectura. no lo cierra: lo hace quien lo abrió.
       devuelve TODO_OK, o el código de error de vectorInsertarFinal (ej: SIN_MEM)
       si no se pudo agregar un registro. */
    char linea[TAM_LINEA];
    tBase_Larga aux;
    int codRet;

    /* lee el encabezado (ANIO;TRIMESTRE;...) y lo descarta */
    fgets(linea,TAM_LINEA,pf);

    while(fgets(linea,TAM_LINEA,pf))
    {
        /* se saltean las lineas vacias o incompletas */
        if(!trozarCampoVariable_BaseLarga(&aux,linea))
            continue;

        codRet = vectorInsertarFinal(infoBS,&aux);

        if (codRet != TODO_OK)
            return codRet;
    }

    return TODO_OK;
}

/* ======================================================================== */
/* OPCIÓN 8 - Promedio trimestral por país                                  */
/* ======================================================================== */

int GenerarArch_Promedio_Pais_Trimestre(Vector* infoBL)
{
    /* agrupa la base larga por país y operación para calcular promedios. */
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

    fprintf(pf,"PAIS_DESC;OPERACION;Suma_monto;cant_trimestres;PROMEDIO\n");
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

    for(i=0;i<(int)vectorGetCantElem(infoPPT);i++)
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

int Cmp_BaseLarga_Pais_Operacion(void* a, void* b)
{
    /* ordena la base larga por país y luego por tipo de operación. */
    tBase_Larga* x = (tBase_Larga*)a;
    tBase_Larga* y = (tBase_Larga*)b;

    if(compararSinMayusculas(x->desc_pais,y->desc_pais)!=0)
        return compararSinMayusculas(x->desc_pais,y->desc_pais);

    if(x->operacion - y->operacion)
        return x->operacion - y->operacion;

    return 0;

}

int Cmp_PPT_DescPais(void* a, void* b)
{
    /* ordena resultados de promedio por descripción de país. */
    tPromedio_Pais_Trimestre* x = (tPromedio_Pais_Trimestre*)a;
    tPromedio_Pais_Trimestre* y = (tPromedio_Pais_Trimestre*)b;
    return compararSinMayusculas(x->desc_pais,y->desc_pais);
}

/* NO SE USA ACTUALMENTE en ninguna parte del programa. */
int Cmp_PPT_PromedioCredito(void* a, void* b)
{
    /* compara resultados de promedio usando el promedio de crédito. */
    tPromedio_Pais_Trimestre* x = (tPromedio_Pais_Trimestre*)a;
    tPromedio_Pais_Trimestre* y = (tPromedio_Pais_Trimestre*)b;
    return (x->promedio_credito > y->promedio_credito) - (x->promedio_credito < y->promedio_credito);
}

/* ======================================================================== */
/* OPCIÓN 9 - Promedio por continente, operación y año                      */
/* ======================================================================== */

int GenerarArch_Promedio_Anio_Continente_Op(Vector* infoBL)
{
    /* agrupa por año, continente y operación para calcular promedios. */
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

int Cmp_Prom_ACO(void* a, void* b)
{
    /* ordena la base larga por año, continente y operación. */
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

/* ======================================================================== */
/* OPCIÓN 10 - Ranking de países según el saldo neto                        */
/* ======================================================================== */

int RankPorSaldoNeto(Vector* infoT)
{
    /* genera un ranking de países según su saldo neto (créditos - débitos).

       pide un año (o 0 para todo el período) y la cantidad de países a mostrar.
       usa las transferencias ya cargadas en memoria, no vuelve a leer el CSV.
       no incluye los códigos agregados W0 (Mundo) y _X (Indeterminado).

       pasos:
       1. pide y valida el año.
       2. ordena infoT por código de país (OJO: ordena el vector original en
          el lugar, igual que las otras opciones) y acumula crédito y débito de
          cada país, cargando un registro por país en el vector 'ranking'.
       3. pide y valida la cantidad de países (no puede superar los disponibles).
       4. ordena 'ranking' por saldo neto, lo muestra y lo destruye.

       devuelve TODO_OK, o SIN_MEM / el código de error de vectorInsertarFinal. */
    size_t i=0;
    size_t ce;
    int anio;
    int cantPaisesListar;
    int codRet;
    float acumC, acumD;
    bool esAgregado, hayDatos;

    Vector* ranking;
    tTransferenciaPorMonto aux;
    tTransferenciaPorMonto* pMonto;
    tTransferencia* vecT;

    if(!infoT)
        return SIN_MEM;

    ce = vectorGetCantElem(infoT);

    printf("\nIngrese un anio (%d-%d) o 0 para todo el periodo: ",ANIO_MENOR,ANIO_ACTUAL);
    anio = ValidarRango_Int(0,ANIO_ACTUAL);
    while(anio != 0 && anio < ANIO_MENOR)
    {
        printf("\nError - Ingrese 0 (todo el periodo) o un anio entre %d y %d: ",ANIO_MENOR,ANIO_ACTUAL);
        anio = ValidarRango_Int(0,ANIO_ACTUAL);
    }

    ranking = vectorCrear(CANT_PAISES,sizeof(tTransferenciaPorMonto));

    if(!ranking)
        return SIN_MEM;

    /* deja juntas las transferencias de un mismo país */
    vectorOrdenar(infoT,Cmp_TCod,MENOR_A_MAYOR);

    vecT = vectorGetElementoPorPos(infoT,i);

    /* cada vuelta de este while procesa un país */
    while(i<ce)
    {
        strcpy(aux.cod_pais,vecT->cod_pais);
        strcpy(aux.desc_pais,vecT->desc_pais);
        acumC = 0;
        acumD = 0;
        hayDatos = false;
        esAgregado = (compararSinMayusculas(aux.cod_pais,COD_MUNDO) == 0 ||
                      compararSinMayusculas(aux.cod_pais,COD_INDETERMINADO) == 0);

        while(i<ce && compararSinMayusculas(aux.cod_pais,vecT->cod_pais) == 0)
        {
            if(!esAgregado && (anio == 0 || vecT->anio == anio))
            {
                hayDatos = true;
                if(vecT->operacion == CREDITO)
                    acumC += vecT->monto;
                else
                    acumD += vecT->monto;
            }
            vecT = vectorGetElementoPorPos(infoT,++i);
        }

        /* sólo entran al ranking los países con movimientos en el año elegido */
        if(hayDatos)
        {
            aux.total_credito = acumC;
            aux.total_debito = acumD;
            aux.total_neto = RedondearDosDecimales(acumC - acumD);

            codRet = vectorInsertarFinal(ranking,&aux);

            if(codRet != TODO_OK)
            {
                vectorDestruir(ranking);
                return codRet;
            }
        }
    }

    if(vectorGetCantElem(ranking) == 0)
    {
        printf("\nNo hay transferencias para el anio indicado.\n");
        vectorDestruir(ranking);
        return TODO_OK;
    }

    printf("\nIngrese la cantidad de paises para mostrar (1-%d): ",(int)vectorGetCantElem(ranking));
    cantPaisesListar = ValidarRango_Int(1,(int)vectorGetCantElem(ranking));

    vectorOrdenar(ranking,Cmp_TPM_Neto,MAYOR_A_MENOR);

    printf("\n%-7s %-9s %-40s %14s %14s %14s\n","puesto","PAIS_COD","PAIS_DESC",
                                                "total_credito","total_debito","saldo_neto");
    for(i=0;i<(size_t)cantPaisesListar;i++)
    {
        pMonto = vectorGetElementoPorPos(ranking,i);
        printf("%-7d %-9s %-40s %14.2f %14.2f %14.2f\n",(int)i+1,pMonto->cod_pais,pMonto->desc_pais,
                                                 pMonto->total_credito,pMonto->total_debito,pMonto->total_neto);
    }

    vectorDestruir(ranking);
    return TODO_OK;
}

int Cmp_TPM_Neto(void* a, void* b)
{
    /* compara dos países por su saldo neto (crédito - débito). si empatan, el
       código de país que va antes alfabéticamente se considera "mayor", para que
       con MAYOR_A_MENOR quede primero en el ranking. */
    tTransferenciaPorMonto* x = (tTransferenciaPorMonto*)a;
    tTransferenciaPorMonto* y = (tTransferenciaPorMonto*)b;

    if(x->total_neto != y->total_neto)
        return (x->total_neto > y->total_neto) - (x->total_neto < y->total_neto);

    return compararSinMayusculas(y->cod_pais,x->cod_pais);
}

/* ======================================================================== */
/* OPCIÓN 11 - Evolución anual de las operaciones de un país                */
/* ======================================================================== */

int EvolucionAnualPais(Vector* infoT)
{
    /* muestra, año por año, los créditos, débitos, saldo neto y variación
       porcentual del saldo de un país elegido por el usuario.

       usa las transferencias ya cargadas en memoria, no vuelve a leer el CSV,
       y no modifica infoT.

       pasos:
       1. pide el código de país.
       2. recorre infoT para comprobar que el país exista y para obtener el
          primer y el último año de toda la base.
       3. por cada año entre esos dos (aunque el país no tenga operaciones, en
          ese caso los importes quedan en cero) acumula crédito y débito, calcula
          el saldo y la variación respecto del año anterior:
              variación % = (saldo actual - saldo anterior) / |saldo anterior| * 100
          - primer año de la serie: no se calcula (SIN_VARIACION).
          - saldo anterior igual a cero: no es calculable (NO_CALCULABLE).

       si el país no existe avisa por pantalla y vuelve al menú.
       devuelve TODO_OK, o SIN_MEM si infoT es nulo. */
    char codIngresado[20];
    char cod[TAM_COD];
    char desc[TAM_DESC_PAIS];
    int c;
    int anio, anioMin, anioMax;
    size_t i;
    size_t ce;
    bool existe=false;
    float credito, debito, saldo, variacion;
    float saldoAnterior=0;

    tTransferencia* vecT;

    if(!infoT)
        return SIN_MEM;

    ce = vectorGetCantElem(infoT);

    if(ce == 0)
    {
        printf("\nNo hay transferencias cargadas.\n");
        return TODO_OK;
    }

    printf("\nIngrese el codigo de un pais (por ejemplo CA): ");
    if(scanf("%19s",codIngresado) != 1)
        codIngresado[0] = '\0';
    /* descarta lo que quedó en el buffer (por ejemplo, el enter) */
    while((c=getchar())!='\n' && c!=EOF);

    vecT = vectorGetElementoPorPos(infoT,0);
    anioMin = vecT->anio;
    anioMax = vecT->anio;

    for(i=0;i<ce;i++)
    {
        vecT = vectorGetElementoPorPos(infoT,i);

        if(vecT->anio < anioMin)
            anioMin = vecT->anio;
        if(vecT->anio > anioMax)
            anioMax = vecT->anio;

        if(!existe && compararSinMayusculas(codIngresado,vecT->cod_pais) == 0)
        {
            existe = true;
            strcpy(cod,vecT->cod_pais);
            strcpy(desc,vecT->desc_pais);
        }
    }

    if(!existe)
    {
        printf("\nEl pais con codigo \"%s\" no existe en la base de datos.\n",codIngresado);
        return TODO_OK;
    }

    printf("\n%-9s %-40s %-6s %14s %14s %14s %14s\n","PAIS_COD","PAIS_DESC","ANIO",
                                                      "total_credito","total_debito","saldo_neto","VAR_PCT_SALDO");

    for(anio=anioMin;anio<=anioMax;anio++)
    {
        credito = 0;
        debito = 0;

        for(i=0;i<ce;i++)
        {
            vecT = vectorGetElementoPorPos(infoT,i);

            if(vecT->anio == anio && compararSinMayusculas(cod,vecT->cod_pais) == 0)
            {
                if(vecT->operacion == CREDITO)
                    credito += vecT->monto;
                else
                    debito += vecT->monto;
            }
        }

        saldo = RedondearDosDecimales(credito - debito);

        printf("%-9s %-40s %-6d %14.2f %14.2f %14.2f ",cod,desc,anio,credito,debito,saldo);

        if(anio == anioMin)
        {
            printf("%14s\n",SIN_VARIACION);
        }
        else if(saldoAnterior == 0)
        {
            printf("%14s\n",NO_CALCULABLE);
        }
        else
        {
            variacion = (saldo - saldoAnterior) / (saldoAnterior < 0 ? -saldoAnterior : saldoAnterior) * 100;
            printf("%14.2f\n",variacion);
        }

        saldoAnterior = saldo;
    }

    return TODO_OK;
}
