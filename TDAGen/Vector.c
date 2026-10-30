#include "Vector.h"


Vector* vectorCrear(size_t tam, size_t tipo)
{
    Vector* v=malloc(sizeof(Vector));
    if(!v)
        return NULL;
    v->vec=malloc(tam*tipo);
    if(!v->vec)
    {
        free(v);
        return NULL;
    }
    v->ce=0;
    v->tam=tam;
    v->tipo=tipo;
    return v;

}
int vectorInsertarFinal(Vector* v, void* elem)
{
    void* aux;


    if(v->ce == v->tam)
    {
        if(v->tam > ((size_t)-1) - INCREMENTO_VEC || v->tam + INCREMENTO_VEC > ((size_t)-1) / v->tipo)
            return SIN_MEM;

        aux = realloc(v->vec, (v->tam + INCREMENTO_VEC) * v->tipo);

        if(!aux)
            return SIN_MEM;

        v->vec = aux;
        v->tam += INCREMENTO_VEC;
    }

    memcpy((char*)v->vec + v->ce * v->tipo, elem, v->tipo);

    v->ce++;

    return TODO_OK;
}
void MostrarVec(Vector* v)
{
    char* temp=(char*)v->vec;
    int i;
    for(i=0;i<(int)v->ce;i++)
    {
        printf("%c ",*(int*)temp);
        temp+=v->tipo;
    }
}

int vectorInsertarEnPos(Vector* v, void* elem, int pos)
{
    void* aux;
    void* destino;

    if(v->ce == v->tam)
    {
        if(v->tam > ((size_t)-1) - INCREMENTO_VEC || v->tam + INCREMENTO_VEC > ((size_t)-1) / v->tipo)
            return SIN_MEM;

        aux = realloc(v->vec, (v->tam + INCREMENTO_VEC) * v->tipo);

        if(!aux)
            return SIN_MEM;

        v->vec = aux;
        v->tam += INCREMENTO_VEC;
    }

    destino = (unsigned char*)v->vec + pos * v->tipo;

    memmove((unsigned char*)destino + v->tipo, destino, (v->ce - pos) * v->tipo);

    memcpy(destino, elem, v->tipo);

    v->ce++;

    return TODO_OK;
}
int vectorOrdInsertar(Vector* v, void* elem, Cmp cmp)
{
    int pos = 0;
    int codRet;
    void* actual;

    if(!v)
        return SIN_MEM;

    while(pos < (int)v->ce)
    {
        actual = vectorGetElementoPorPos(v, pos);

        if(cmp(elem, actual) <= 0)
            break;

        pos++;
    }

    codRet = vectorInsertarEnPos(v, elem, pos);

    return codRet;
}

void vectorOrdenar(Vector* v,Cmp cmp,int opcion)
{
    char* DirIni=(char*)v->vec;
    char* DirUlt=(char*)v->vec+v->ce*v->tipo;
    void* menor;
    char* i;
    for(i=DirIni;i<DirUlt;i+=v->tipo)
    {
        switch(opcion)
        {
            case 1: menor=Menor(v,i,cmp);
                    break;
            case 2: menor=Mayor(v,i,cmp);
                    break;
            default: /* opcion invalida: no se ordena */
                    return;
        }


        Swap(menor,i,v->tipo);

    }
}
void* Menor(Vector* v, void* actual,Cmp cmp)
{
    void* menor=actual;
    char* j=(char*)actual+v->tipo;
    char* fin=(char*)v->vec+v->ce*v->tipo;
    for(;j<fin;j+=v->tipo)
            if(cmp(menor,j)>0)
                menor=j;
    return menor;
}
void* Mayor(Vector* v, void* actual,Cmp cmp)
{
    void* mayor=actual;
    char* j=(char*)actual+v->tipo;
    char* fin=(char*)v->vec+v->ce*v->tipo;
    for(;j<fin;j+=v->tipo)
            if(cmp(mayor,j)<0)
                mayor=j;
    return mayor;
}

void Swap(void* a, void* b, size_t tipo)
{
    char* x=(char*)a;
    char* y=(char*)b;
    char temp;
    while(tipo--)
    {
        temp=*x;
        *x=*y;
        *y=temp;
        x++;
        y++;
    }
}
int vectorDesordBuscar(Vector* v, void* elem,Cmp cmp)
{
    char* DirIni=(char*)v->vec;
    char* DirFin=(char*)v->vec+v->ce*v->tipo;
    int pos=-1;

    while(DirIni<DirFin && cmp(elem,DirIni)!=0)
        DirIni+=v->tipo;
    if(DirIni<DirFin && cmp(elem,DirIni)==0)
        pos=(int)(DirIni-(char*)v->vec)/v->tipo;
    return pos;
}
int vectorOrdBuscar(Vector* v, void* elem,Cmp cmp)
{
    char* LimI=(char*)v->vec;
    char* LimS;
    char* medio;
    bool encontrado=false;
    int pos=-1;

    if(v->ce==0)
        return -1;

    LimS=(char*)v->vec+(v->ce-1)*v->tipo;
    while(!encontrado && LimI<=LimS)
    {
       medio = LimI + ((LimS - LimI) / v->tipo / 2) * v->tipo;


        if(cmp(medio,elem)==0)
        {
            encontrado=true;
            pos=(int)(medio-(char*)v->vec)/v->tipo;
        }
        if(cmp(medio,elem)>0)
            LimS=medio-v->tipo;

        if(cmp(medio,elem)<0)
            LimI=medio+v->tipo;
    }
    return pos;

}
bool vectorBorrarPorPos(Vector* v, int pos)
{
    if(pos<0 || pos>=(int)v->ce)
        return false;

    if(pos<(int)v->ce-1)
        memmove((char*)v->vec+v->tipo*pos,
                (char*)v->vec+v->tipo*(pos+1),
                v->tipo*(v->ce-pos-1));
    v->ce--;
    return true;
}
bool vectorDesordBorrar(Vector*v, void* elem,Cmp cmp)
{
    int pos=vectorDesordBuscar(v,elem,cmp);
    return vectorBorrarPorPos(v,pos);
}
bool vectorOrdBorrar(Vector*v, void* elem,Cmp cmp)
{
    int pos=vectorOrdBuscar(v,elem,cmp);
    return vectorBorrarPorPos(v,pos);
}
bool vectorBorrarTodaAparicion(Vector* v, void* elem,Cmp cmp)
{
    int i;
    for(i=(int)v->ce-1;i>=0;i--)
    {
        if(cmp(vectorGetElementoPorPos(v,i),elem)==0)
            vectorBorrarPorPos(v,i);
    }
    return true;
}
void vectorDestruir(Vector* v)
{
    /* libero recursos antes cuando falla malloc */
    if(!v)
        return;
    v->ce=0;
    v->tam=0;
    free(v->vec);
    free(v);
}

void* vectorGetElementoPorPos(Vector* v, int pos)
{
    return (char*)v->vec+pos*v->tipo;
}

size_t vectorGetCantElem(Vector* v)
{
    return v->ce;
}