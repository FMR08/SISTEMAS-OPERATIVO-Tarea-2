#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    bool valid;
    unsigned long nvp;
    int ref;
} frame_t;

static int calcular_b(int page_size) {
    int b = 0;
    int tmp = page_size;

    if (tmp <= 0) return -1;

    while ((tmp & 1) == 0) {
        b++;
        tmp >>= 1;
    }
    if (tmp != 1) {
        return -1; 
    }
    return b;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr,
                "Uso: %s Nmarcos tamañomarco [--verbose] traza.txt\n",
                argv[0]);
        return 1;
    }

    int argi = 1;
    int Nmarcos = atoi(argv[argi++]);
    int page_size = atoi(argv[argi++]);

    if (Nmarcos <= 0 || page_size <= 0) {
        fprintf(stderr, "Error: Nmarcos y tamañomarco deben ser > 0\n");
        return 1;
    }

    bool verbose = false;
    const char *trace_file = NULL;

    if (argc - argi == 2) {
        if (strcmp(argv[argi], "--verbose") == 0) {
            verbose = true;
            trace_file = argv[argi + 1];
        } else {
            fprintf(stderr,
                    "Error de argumentos. Forma: %s Nmarcos tamañomarco [--verbose] traza.txt\n",
                    argv[0]);
            return 1;
        }
    } else if (argc - argi == 1) {
        trace_file = argv[argi];
    } else {
        fprintf(stderr,
                "Error de argumentos. Forma: %s Nmarcos tamañomarco [--verbose] traza.txt\n",
                argv[0]);
        return 1;
    }

    int b = calcular_b(page_size);
    if (b < 0) {
        fprintf(stderr, "Error: tamañomarco (%d) debe ser potencia de 2\n", page_size);
        return 1;
    }

    unsigned long mask = (unsigned long)page_size - 1UL;

    FILE *f = fopen(trace_file, "r");
    if (!f) {
        perror("Error abriendo archivo de traza");
        return 1;
    }

    frame_t *frames = calloc(Nmarcos, sizeof(frame_t));
    if (!frames) {
        perror("calloc");
        fclose(f);
        return 1;
    }

    unsigned long total_refs = 0;
    unsigned long page_faults = 0;
    int clock_hand = 0;

    char line[256];

    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '#')
            continue;

        char *endptr = NULL;
        unsigned long dv = strtoul(p, &endptr, 0);
        if (endptr == p) {
            continue;
        }

        total_refs++;

        unsigned long offset = dv & mask;
        unsigned long nvp   = dv >> b;

        int marco = -1;
        bool hit = false;

        for (int i = 0; i < Nmarcos; i++) {
            if (frames[i].valid && frames[i].nvp == nvp) {
                hit = true;
                marco = i;
                frames[i].ref = 1;
                break;
            }
        }

        if (!hit) {
            page_faults++;
            int free_frame = -1;
            for (int i = 0; i < Nmarcos; i++) {
                if (!frames[i].valid) {
                    free_frame = i;
                    break;
                }
            }

            if (free_frame != -1) {
                marco = free_frame;
            } else {
                while (true) {
                    if (!frames[clock_hand].valid || frames[clock_hand].ref == 0) {
                        marco = clock_hand;
                        break;
                    } else {
                        frames[clock_hand].ref = 0;
                        clock_hand = (clock_hand + 1) % Nmarcos;
                    }
                }
            }

            frames[marco].valid = true;
            frames[marco].nvp   = nvp;
            frames[marco].ref   = 1;

            clock_hand = (marco + 1) % Nmarcos;
        }

        unsigned long df = ((unsigned long)marco << b) | offset;

        if (verbose) {
            printf("DV=0x%lx nvp=%lu offset=%lu %s marco=%d DF=0x%lx\n",
                   dv, nvp, offset,
                   hit ? "HIT" : "FALLO",
                   marco, df);
        }
    }

    fclose(f);

    double tasa = 0.0;
    if (total_refs > 0) {
        tasa = (double)page_faults / (double)total_refs;
    }

    printf("Totales:\n");
    printf("  Referencias    : %lu\n", total_refs);
    printf("  Fallos de pagina: %lu\n", page_faults);
    printf("  Tasa de fallos : %.6f\n", tasa);

    free(frames);
    return 0;
}
