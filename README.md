# SISTEMA OPERATIVO Tarea 2

## Requisitos

En WSL o Linux:
sudo apt update
sudo apt install -y build-essential

## Como ejecutar

En la terminal dentro de la carpeta del proyecto:
```
gcc -Wall -pthread -o test_barrier main.c barrier.c
```

Como ejecutar `test_barrier`:

```
./test_barrier
./test_barrier <N_hebras> <E_etapas>

```
## Simulador de Memoria Virtual (`sim`)

En la terminal dentro de la carpeta del proyecto:

```
gcc -Wall -o sim sim.c

```
Como ejecutar `./sim` 


```
./sim 16 8 traces/trace1
```
Variables:

16 número de marcos físicos (8, 16, 32, etc.).
8 tamaño del marco en bytes
traces/trace1.txt archivo de traza a procesar


Datos de prueba:

```
# trace1.txt (page_size = 8)
./sim 8 8 traces/trace1.txt
./sim 16 8 traces/trace1.txt
./sim 32 8 traces/trace1.txt

# trace2.txt (page_size = 4096)
./sim 8 4096 traces/trace2.txt
./sim 16 4096 traces/trace2.txt
./sim 32 4096 traces/trace2.txt

```
## Informe
https://docs.google.com/document/d/1dDY59cnxSt_-cC4ywc4e3ESlePzasIGNbb15yzi88w0/edit?tab=t.0