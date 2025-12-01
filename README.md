# SISTEMA OPERATIVO Tarea 2

## Requisitos

En WSL o Linux:
sudo apt update
sudo apt install -y build-essential

## Como ejecutar

En la terminal dentro de la carpeta del proyecto:
```
gcc -Wall -pthread -o test_barrier main.c barrier.c
./test_barrier
./test_barrier <N_hebras> <E_etapas>

```
