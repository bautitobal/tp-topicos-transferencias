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
bool vectorInsertarFinal(Vector* v, void* elem)
{
    void* aux;
    if(v->ce==v->tam)
    {
        aux=realloc(v->vec,(v->tam+INCREMENTO_VEC)*v->tipo);
        if(!aux)
            return false;

        v->vec=aux;
        v->tam+=INCREMENTO_VEC;
    }

    void* temp=v->vec;
    memcpy(temp+v->ce*v->tipo,elem,v->tipo);
    v->ce++;
    return true;
}
void MostrarVec(Vector* v)
{
    void* temp=v->vec;
    int i;
    for(i=0;i<v->ce;i++)
    {
        printf("%c ",*(int*)temp);
        temp+=v->tipo;
    }
}

bool vectorInsertarEnPos(Vector* v, void* elem,int pos)
{
    if(pos<0 || pos>v->ce)
        return false;

    void* tvec;
    if(v->ce==v->tam)
    {
        tvec=realloc(v->vec,(v->tam+INCREMENTO_VEC)*v->tipo);
        if(!tvec)
            return false;

        v->vec=tvec;
        v->tam+=INCREMENTO_VEC;
    }
    void* DirIns=v->vec+pos*v->tipo;
    void* DirUlt=v->vec+v->ce*v->tipo;
    void* i;

    for(i=DirUlt;i>=DirIns;i-=v->tipo)
        memcpy(i+v->tipo,i,v->tipo);

    memcpy(DirIns,elem,v->tipo);
    v->ce++;
    return true;


}
bool vectorOrdInsertar(Vector* v, void* elem,Cmp cmp)
{
    void* DirIni=v->vec;
    void* DirUlt=v->vec+v->ce*v->tipo-v->tipo;


    while(DirIni<=DirUlt && cmp(elem,DirIni)>0)
        DirIni+=v->tipo;
    return vectorInsertarEnPos(v,elem,(DirIni-v->vec)/v->tipo);
}

void vectorOrdenar(Vector* v,Cmp cmp,int opcion)
{
    void* DirIni=v->vec;
    void* DirUlt=v->vec+v->ce*v->tipo;
    void* menor;
    void* i;
    for(i=DirIni;i<DirUlt;i+=v->tipo)
    {
        switch(opcion)
        {
            case 1: menor=Menor(v,i,cmp);
                    break;
            case 2: menor=Mayor(v,i,cmp);
                    break;
        }


        Swap(menor,i,v->tipo);

    }
}
void* Menor(Vector* v, void* actual,Cmp cmp)
{
    void* menor=actual;
    void* j;
    for(j=actual+v->tipo;j<v->vec+v->ce*v->tipo;j+=v->tipo)
            if(cmp(menor,j)>0)
                menor=j;
    return menor;
}
void* Mayor(Vector* v, void* actual,Cmp cmp)
{
    void* mayor=actual;
    void* j;
    for(j=actual+v->tipo;j<v->vec+v->ce*v->tipo;j+=v->tipo)
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
    void* DirIni=v->vec;
    void* DirFin=v->vec+v->ce*v->tipo;
    int pos=-1;

    while(DirIni<DirFin && cmp(elem,DirIni)!=0)
        DirIni+=v->tipo;
    if(DirIni<DirFin && cmp(elem,DirIni)==0)
        pos=(DirIni-v->vec)/v->tipo;
    return pos;
}
int vectorOrdBuscar(Vector* v, void* elem,Cmp cmp)
{
    void* LimI=v->vec;
    void* LimS=v->vec+(v->ce-1)*v->tipo;
    void* medio;
    bool encontrado=false;
    int pos=-1;

    while(!encontrado && LimI<=LimS)
    {
       medio = LimI + ((LimS - LimI) / v->tipo / 2) * v->tipo;


        if(cmp(medio,elem)==0)
        {
            encontrado=true;
            pos=(medio-v->vec)/v->tipo;
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
    if(pos<0 || pos>=v->ce)
        return false;

    void* DirIni=v->vec+v->tipo*pos;
    void* DirFin=v->vec+v->tipo*v->ce;
    void* i;

    for(i=DirIni;i<DirFin;i+=v->tipo)
        memcpy(i,i+v->tipo,v->tipo);
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
    void* DirIni=v->vec;
    void* DirFin=v->vec+v->tipo*v->ce-v->tipo;
    void* i;
    for(i=DirFin;i>=DirIni;i-=v->tipo)
    {
        if(cmp(i,elem)==0)
            vectorBorrarPorPos(v,(i-v->vec)/v->tipo);
    }
    return true;
}
void vectorDestruir(Vector* v)
{
    v->ce=0;
    v->tam=0;
    free(v->vec);
    free(v);
}