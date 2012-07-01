#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * SEGMENT refers to the position of the 'pixel' within the display
 * The number is the GPIO pin number.
 */
#define SEGMENT_TOP 24
#define SEGMENT_UPPER_LEFT 23
#define SEGMENT_UPPER_RIGHT 25
#define SEGMENT_MIDDLE 22
#define SEGMENT_LOWER_LEFT 4
#define SEGMENT_LOWER_RIGHT 18
#define SEGMENT_BOTTOM 17
#define SEGMENT_DECIMAL 21

#define TURN_SEGEMENT_ON "0"
#define TURN_SEGEMENT_OFF "1"

/*
 * There are 8 segments, each with 2 values.
 * The most significant bit refers to SEGMENT_TOP.
 * The least significant bit refers to SEGMENT_DECIMAL.
 * 1 means the segment is on, and 0 means the segment is off.
 */
#define DIGIT_1 0b00100100
#define DIGIT_2 0b10111010
#define DIGIT_3 0b10110110
#define DIGIT_4 0b01110100
#define DIGIT_5 0b11010110
#define DIGIT_6 0b11011110
#define DIGIT_7 0b10100100
#define DIGIT_8 0b11111110
#define DIGIT_9 0b11110100
#define DIGIT_0 0b11101110

int all_segments[] = { \
SEGMENT_TOP, \
SEGMENT_UPPER_LEFT, \
SEGMENT_UPPER_RIGHT, \
SEGMENT_MIDDLE, \
SEGMENT_LOWER_LEFT, \
SEGMENT_LOWER_RIGHT, \
SEGMENT_BOTTOM, \
SEGMENT_DECIMAL, \
0 \
};

int digit_segments[] = { \
DIGIT_0, \
DIGIT_1, \
DIGIT_2, \
DIGIT_3, \
DIGIT_4, \
DIGIT_5, \
DIGIT_6, \
DIGIT_7, \
DIGIT_8, \
DIGIT_9, \
0 \
};

FILE *segment_files[sizeof(all_segments)/sizeof(int)];

void export_or_unexport(FILE *file) {
    int i;
    for (i=0; all_segments[i] != 0; i++) {
        int gpio = all_segments[i];
        fprintf(file, "%d\n", gpio);
        if (fflush(file)==EOF) {
            fprintf(stderr, "gpio %d\n", gpio);
            perror(NULL);
        }
    }
}

void set_directions() {
    int i;
    FILE *direction_file;
    char file_name[sizeof("/sys/class/gpio/gpioXX/direction")];
    for (i=0; all_segments[i] != 0; i++) {
        int gpio = all_segments[i];
        snprintf(file_name, sizeof(file_name), "/sys/class/gpio/gpio%d/direction", gpio);
        direction_file = fopen(file_name, "w");
        if (direction_file == NULL) {
            fprintf(stderr, "can't open %s\n", file_name);
            perror(NULL);
            exit(1);
        }
        fprintf(direction_file, "out");
        if (fclose(direction_file)==EOF) {
            fprintf(stderr, "can't close %s\n", file_name);
            perror(NULL);
        }
    }
}

void open_gpio_files() {
    int i;
    char file_name[sizeof("/sys/class/gpio/gpioXX/value")];
    FILE *gpio_file;
    for (i=0; all_segments[i] != 0; i++) {
        int gpio = all_segments[i];
        snprintf(file_name, sizeof(file_name), "/sys/class/gpio/gpio%d/value", gpio);
        gpio_file = fopen(file_name, "w");
        if (gpio_file == NULL) {
            fprintf(stderr, "can't open %s\n", file_name);
            perror(NULL);
            exit(1);
        }
        segment_files[i] = gpio_file;
    }
    segment_files[i] = NULL;
}

void close_gpio_files() {
    int i;
    FILE *gpio_file;
    for (i=0; segment_files[i] != NULL; i++) {
        gpio_file = segment_files[i];
        if (fclose(gpio_file) == EOF) {
            fprintf(stderr, "can't close segment file #%d\n", i);
            perror(NULL);
        }
    }
}

void display_digit(int digit) {
    if (digit < 0 || digit > 9) {
        return;
    }
    int mask = digit_segments[digit];
    int segment_shift;
    int segment_counter = 0;
    for (segment_shift = 0b10000000; segment_shift > 0; segment_shift = segment_shift >> 1, segment_counter++) {
        FILE *segment_file = segment_files[segment_counter];
        if (segment_shift == (mask & segment_shift)) {
            fprintf(segment_file, TURN_SEGEMENT_ON);
        } else {
            fprintf(segment_file, TURN_SEGEMENT_OFF);
        }
        fflush(segment_file);
    }
}

int main(int argc, char** argv)
{
    FILE *export = fopen("/sys/class/gpio/export", "w");
    if (export == NULL) {
        perror(NULL);
        exit(1);
    }

    FILE *unexport = fopen("/sys/class/gpio/unexport", "w");
    if (unexport == NULL) {
        perror(NULL);
        exit(1);
    }

    export_or_unexport(export);
    set_directions();
    open_gpio_files();
    int i;
    for (i=0; i<=9; i++) {
        display_digit(i);
        sleep(1);
    }
    close_gpio_files();
    export_or_unexport(unexport);


    if (fclose(export)==EOF) {
        perror(NULL);
    }

    if (fclose(unexport)==EOF) {
        perror(NULL);
    }

    return 0;
}
