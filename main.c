#include <stdio.h>
#include <stdlib.h>

const size_t F = sizeof(float);

extern void start(int width, int height, float *matrix, float *heaters, float *coolers, float weight) {}

extern void step() {}


// [beg; end)
static void print_range(float *beg, float *end) {
    for(float *i = beg; i < end; i++) {
        printf("%f ", *i);
    }
}


static void print_state(int width, int height, float *matrix, float *heaters, float *coolers, int steps_done) {
    printf("After %d steps:\n\n", steps_done);
    printf("X ");
    print_range(heaters, heaters + width);
    printf("X\n\n");

    for(int row = 0; row < height; row++) {
        printf("%f  |  ", coolers[row]);
        print_range(matrix + row * width, matrix + (row + 1) * width);
        printf(" |  %f\n", coolers[row]);

    }

    printf("\nX ");
    print_range(heaters, heaters + width);
    printf("X\n\n\n");
}


int main(int argc, char **argv) {
    int retval = 1;
    float weight;
    int height, width, steps;
    FILE *file;
    float *matrix = NULL;
    float *heaters = NULL;
    float *coolers = NULL;

    if(argc < 4) {
        printf("usage: %s filepath weight steps\n", argc > 0 ? argv[0] : "simulate");
        goto ret;
    }

    steps = atoi(argv[3]);
    if(steps <= 0) {
        printf("please provide positive number of steps");
        goto ret;
    }
    weight = strtof(argv[2], NULL);

    if(!(file = fopen(argv[1], "r"))) {
        perror("open");
        goto ret;
    }

    if(2 > fscanf(file, "%d%d", &width, &height)) {
        printf("incorrect input file");
        goto close_file;
    }

    if(!(matrix = malloc(F * width * height)) || !(heaters = malloc(F * width)) || !(coolers = malloc(F * height))) {
        perror("malloc");
        goto free_buffers;
    }

    for (size_t i = 0; i < width * height; i++) {
        if(1 > fscanf(file, "%f", matrix + i)) {
            printf("incorrect input file");
            goto free_buffers;
        }
    }

    for (size_t i = 0; i < width; i++) {
        if(1 > fscanf(file, "%f", heaters + i)) {
            printf("incorrect input file");
            goto free_buffers;
        }
    }

    for (size_t i = 0; i < height; i++) {
        if(1 > fscanf(file, "%f", coolers + i)) {
            printf("incorrect input file");
            goto free_buffers;
        }
    }

    start(width, height, matrix, heaters, coolers, weight);

    print_state(width, height, matrix, heaters, coolers, 0);

    for(int i = 0; i < steps; i++) {
        while(getchar()!='\n');
        step();
        print_state(width, height, matrix, heaters, coolers, i + 1);
    }


    retval = 0;

    free_buffers:
        free(matrix);
        free(heaters);
        free(coolers);
    close_file:
        fclose(file);
    ret:
        return retval;
}
