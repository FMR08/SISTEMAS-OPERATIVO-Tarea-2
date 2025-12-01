#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    int valid;
    unsigned long nvp;
    int ref;
} Frame;

int main(int argc, char *argv[]) {
    if (argc < 4) return 1;

    int N = atoi(argv[1]);
    int page_size = atoi(argv[2]);
    int verbose = 0;
    char *trace_file;
    int argi = 3;

    if (argc == 5 && strcmp(argv[3], "--verbose") == 0) {
        verbose = 1;
        trace_file = argv[4];
    } else {
        trace_file = argv[3];
    }

    int b = 0;
    int tmp = page_size;
    while ((tmp & 1) == 0) {
        b++;
        tmp >>= 1;
    }

    unsigned long mask = (unsigned long)page_size - 1UL;

    FILE *f = fopen(trace_file, "r");
    if (!f) return 1;

    Frame *frames = calloc(N, sizeof(Frame));
    if (!frames) return 1;

    unsigned long total = 0;
    unsigned long faults = 0;
    int clock_hand = 0;
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        unsigned long dv = strtoul(line, NULL, 0);
        unsigned long offset = dv & mask;
        unsigned long nvp = dv >> b;

        total++;

        int marco = -1;
        int hit = 0;

        for (int i = 0; i < N; i++) {
            if (frames[i].valid && frames[i].nvp == nvp) {
                hit = 1;
                marco = i;
                frames[i].ref = 1;
                break;
            }
        }

        if (!hit) {
            faults++;

            int free_frame = -1;
            for (int i = 0; i < N; i++) {
                if (!frames[i].valid) {
                    free_frame = i;
                    break;
                }
            }

            if (free_frame != -1) {
                marco = free_frame;
            } else {
                for (;;) {
                    if (!frames[clock_hand].valid || frames[clock_hand].ref == 0) {
                        marco = clock_hand;
                        break;
                    } else {
                        frames[clock_hand].ref = 0;
                        clock_hand = (clock_hand + 1) % N;
                    }
                }
            }

            frames[marco].valid = 1;
            frames[marco].nvp = nvp;
            frames[marco].ref = 1;
            clock_hand = (marco + 1) % N;
        }

        unsigned long df = ((unsigned long)marco << b) | offset;

        if (verbose) {
            printf("DV=0x%lx nvp=%lu offset=%lu %s marco=%d DF=0x%lx\n",
                   dv, nvp, offset, hit ? "HIT" : "FALLO", marco, df);
        }
    }

    fclose(f);

    double tasa = (total > 0) ? (double)faults / (double)total : 0.0;

    printf("Referencias: %lu\n", total);
    printf("Fallos: %lu\n", faults);
    printf("Tasa: %.6f\n", tasa);

    free(frames);
    return 0;
}
