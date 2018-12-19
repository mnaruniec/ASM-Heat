// author: Michal Naruniec (mn360386)
// Assembly Language Programming assignment #2


#include <stdio.h>
#include <stdlib.h>

const size_t F = sizeof(float);

extern void start(int width, int height, float *matrix, float *heaters, float *coolers, float weight);

extern void step();


// [beg; end)
static void print_range(float *beg, float *end) {
    for(float *i = beg; i < end; i++) {
        printf("%f ", *i);
    }
}


static void print_state(int width, int height, float *matrix, int steps_done) {
    int true_width = (width + 4);

    printf("After %d steps:\n\n", steps_done);
    printf("X  |  ");
    print_range(matrix + 1, matrix + width + 1);
    printf(" |  X\n\n");

    for(int row = 0; row < height; row++) {
        printf("%f  |  ", matrix[(row + 1) * true_width]);
        print_range(matrix + (row + 1) * true_width + 1, matrix + (row + 1) * true_width + width + 1);
        printf(" |  %f\n", matrix[(row + 1) * true_width]);

    }

    printf("\nX  |  ");
    print_range(matrix + 1, matrix + width + 1);
    printf(" |  X\n\n\n");
}


int main(int argc, char **argv) {
    int retval = 1;
    float weight;
    int height, width, steps, true_size, true_width;
    FILE *file;
    float *matrix = NULL;
    float *coolers = NULL;

    if(argc < 4) {
        printf("usage: %s filepath weight steps\n", argc > 0 ? argv[0] : "simulate");
        goto ret;
    }

    steps = atoi(argv[3]);
    if(steps <= 0) {
        printf("please provide positive number of steps\n");
        goto ret;
    }
    weight = strtof(argv[2], NULL);

    if(!(file = fopen(argv[1], "r"))) {
        perror("fopen");
        goto ret;
    }

    if(2 > fscanf(file, "%d%d", &width, &height)) {
        printf("incorrect input file\n");
        goto close_file;
    }

    true_width = (width + 4);
    true_size = true_width * (height * 2 + 2);
    if(width <= 0 || height <= 0 || true_width <= 0 || true_size <= 0) {
        printf("incorrect matrix size\n");
        goto close_file;
    }

    if(!(matrix = malloc(F * true_size))
       || !(coolers = malloc(F * height))) {
        perror("malloc");
        goto free_buffers;
    }

    for(int i = 0; i < true_size; i++) {
        matrix[i] = 0.0;
    }

    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            if(1 > fscanf(file, "%f", matrix + (row + 1) * true_width + col + 1)) {
                printf("incorrect input file\n");
                goto free_buffers;
            }
        }
    }

    for (size_t i = 0; i < width; i++) {
        if(1 > fscanf(file, "%f", matrix + i + 1)) {
            printf("incorrect input file\n");
            goto free_buffers;
        }
        matrix[(height + 1) * true_width + i + 1] = matrix[i + 1];
    }

    for (size_t i = 0; i < height; i++) {
        if(1 > fscanf(file, "%f", coolers + i)) {
            printf("incorrect input file\n");
            goto free_buffers;
        }
        matrix[(i + 1) * true_width] = matrix[(i + 1) * true_width + width + 1] = coolers[i];
    }


    start(width, height, matrix, matrix + 1, coolers, weight);
    print_state(width, height, matrix, 0);

    for(int i = 0; i < steps; i++) {
        while(getchar()!='\n');
        step();
        print_state(width, height, matrix, i + 1);
    }


    retval = 0;

    free_buffers:
        free(matrix);
        free(coolers);
    close_file:
        fclose(file);
    ret:
        return retval;
}
