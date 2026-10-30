# Transferencias Personales

Trabajo práctico grupal de la materia **Tópicos de Programación** de la carrera
**Ingeniería Informática** de la **Universidad Nacional de La Matanza**.

## Descripción

Aplicación desarrollada en C para cargar y analizar transferencias personales
internacionales a partir de archivos separados por punto y coma (`;`). La
información se almacena en vectores genéricos y se generan distintos reportes
por país, continente, trimestre, año y tipo de operación.

Las operaciones disponibles incluyen:

1. Totales de transferencias por país.
2. Totales por continente.
3. Saldo trimestral por país.
4. Totales anuales y variación interanual.
5. Ranking de países por crédito o débito.
6. Participación porcentual por continente.
7. Generación de la base larga.
8. Promedio trimestral por país.
9. Promedio por continente, operación y año.
10. Salir.

## Integrantes

- Bautista Tobal
- Santino Zurita
- Juan Martino Soave
- Joaquín Romano

## Requisitos

- Code::Blocks con compilador GCC, o GCC instalado en el sistema.
- Estándar de compilación configurado en los proyectos: C90.

## Ejecución con Code::Blocks

1. Abrir [`TP-Topicos-Transferencias.workspace`](TP-Topicos-Transferencias.workspace).
2. Compilar el workspace completo, respetando el orden de los proyectos:
   `TDAGen`, `Transferencias` y `Programa`.
3. Ejecutar el proyecto `Programa`. Los proyectos `TDAGen` y `Transferencias`
   generan bibliotecas y no son aplicaciones ejecutables.

El ejecutable se genera en `Programa/bin/Debug/Programa` en Linux y como
`Programa/bin/Debug/Programa.exe` en Windows. El programa utiliza rutas
relativas para leer los archivos de entrada (`../transferencias_personales_clean.csv`
y `../paises_continentes.csv`), por lo que el directorio de trabajo debe ser
`Programa`, no `Programa/bin/Debug`.

También se puede compilar directamente desde la raíz del repositorio:

```bash
gcc -std=c90 -Wall -Wextra \
  Programa/main.c Transferencias/transferencias.c TDAGen/Vector.c \
  -I. -ITDAGen -ITransferencias -o programa
```

La compilación finaliza correctamente. El código todavía presenta advertencias
del compilador relacionadas principalmente con declaraciones posteriores a
sentencias, conversiones de tipos y aritmética con punteros genéricos.

## Archivos de entrada

- [`transferencias_personales_clean.csv`](transferencias_personales_clean.csv):
  contiene las transferencias originales.
- [`paises_continentes.csv`](paises_continentes.csv):
  relaciona el código de cada país con su descripción y continente.

Los archivos utilizan `;` como separador y deben incluir una línea de
encabezado. Las transferencias deben respetar el siguiente formato:

```text
ANIO;TRIMESTRE;PAIS_COD;PAIS_DESC;OPERACION;MONTO
```

La operación debe ser `C` para crédito o `D` para débito, y el trimestre debe
estar entre 1 y 4. El archivo de países utiliza el formato:

```text
PAIS_COD;PAIS_DESC;CONTINENTE
```

El cargador saltea el encabezado y espera que las líneas restantes respeten
estos formatos; no realiza una validación completa de registros inválidos.

## Archivos generados

El programa genera reportes en la carpeta desde la que se ejecuta, normalmente
`Programa/`:

- `transferencias_totales_paises.csv`
- `transferencias_totales_continentes.csv`
- `transferencia_totales_trimestres.txt`
- `transferencia_totales_anio.csv`
- `transferencias_totales_paises_operacion.csv`
- `transferencias_totales_continente_porcentaje.dat`
- `transferencias_base_larga.csv`
- `transferencias_promedio_pais_trimestre.csv`
- `transferencias_promedio_continente_anio_op.csv`

La opción 7 genera la base larga y las opciones 8 y 9 utilizan ese archivo
intermedio para producir sus reportes.

## Estructura del proyecto

```text
Programa/       Punto de entrada y menú principal
Transferencias/ Lógica de carga, procesamiento y generación de reportes
TDAGen/         TDA vector genérico y funciones de ordenamiento
```

## Validaciones y entrada por teclado

El menú valida que la opción esté entre 1 y 10. Las opciones que requieren un
año, una cantidad de países o un tipo de operación también solicitan valores
dentro de los rangos definidos por el programa. Los archivos de entrada deben
estar presentes y ser legibles antes de iniciar la ejecución.
