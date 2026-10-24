# Transferencias Personales

Trabajo práctico grupal de la materia **Tópicos de Programación** de la carrera
**Ingeniería Informática** de la **Universidad Nacional de La Matanza**.

## Descripción

Aplicación desarrollada en C para analizar transferencias personales
internacionales a partir de archivos separados por punto y coma (`;`).
El programa carga la información en memoria y genera reportes estadísticos
por país, continente, trimestre, año y tipo de operación.

Las operaciones disponibles incluyen:

- totales de transferencias por país;
- totales por continente;
- saldo trimestral por país;
- totales anuales y variación interanual;
- ranking de países por crédito o débito;
- participación porcentual por continente;
- generación del archivo intermedio de base larga;
- promedios trimestrales por país;
- promedios por continente, operación y año.

## Integrantes

- Bautista Tobal
- Santino Zurita
- Juan Martino Soave
- Joaquín Romano

## Requisitos

- Code::Blocks con compilador GCC, o GCC instalado en el sistema.
- Estándar de compilación utilizado por el proyecto: C90.

## Ejecución con Code::Blocks

1. Abrir [`TP-Topicos-Transferencias.workspace`](TP-Topicos-Transferencias.workspace).
2. Compilar el workspace completo.
3. Ejecutar el proyecto `Programa`.

El programa utiliza rutas relativas. Por eso, debe ejecutarse desde la carpeta
`Programa/bin/Debug` o configurarse en Code::Blocks para que el directorio de
trabajo contenga los archivos de entrada.

## Archivos de entrada

- [`transferencias_personales_clean.csv`](transferencias_personales_clean.csv):
  contiene las transferencias originales.
- [`paises_continentes.csv`](paises_continentes.csv):
  relaciona el código de cada país con su descripción y continente.

Los archivos utilizan `;` como separador. Las transferencias deben respetar el
siguiente formato:

```text
ANIO;TRIMESTRE;PAIS_COD;PAIS_DESC;OPERACION;MONTO
```

La operación debe ser `C` para crédito o `D` para débito, y el trimestre debe
estar entre 1 y 4. Las líneas inválidas se descartan durante la carga.

## Archivos generados

El programa genera reportes en la carpeta desde la que se ejecuta:

- `transferencias_totales_paises.csv`
- `transferencias_totales_continentes.csv`
- `transferencia_totales_trimestres.txt`
- `transferencia_totales_anio.csv`
- `transferencias_totales_paises_operacion.csv`
- `transferencias_totales_continente_porcentaje.dat`
- `transferencias_base_larga.csv`
- `transferencias_promedio_pais_trimestre.csv`
- `transferencias_promedio_continente_anio_op.csv`

## Estructura del proyecto

```text
Programa/       Punto de entrada y menú principal
Transferencias/ Lógica de carga, procesamiento y generación de reportes
TDAGen/         TDA vector genérico y funciones de ordenamiento
```

## Validaciones

La aplicación valida los datos de entrada antes de incorporarlos a los
vectores. También valida las opciones ingresadas por teclado y evita loops
infinitos cuando se introducen valores con formato incorrecto.
