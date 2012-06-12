#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SEGMENT_TOP 24
#define SEGMENT_UPPER_LEFT 23
#define SEGMENT_UPPER_RIGHT 25
#define SEGMENT_MIDDLE 22
#define SEGMENT_LOWER_LEFT 4
#define SEGMENT_LOWER_RIGHT 18
#define SEGMENT_BOTTOM 17
#define SEGMENT_DECIMAL 21

#define TURN_SEGEMENT_ON 0
#define TURN_SEGEMENT_OFF 1

int ALL_SEGMENTS[] = { \
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



int export_fd;
int unexport_fd;

void export_unexport_gpio(int gpio, int fd) {
    char gpio_s[3];
    int gpio_s_length;
    gpio_s_length = snprintf(gpio_s, sizeof(gpio_s), "%d", gpio);

    int wrote = 0;
    while (wrote < gpio_s_length) {
        wrote = write(fd, gpio_s + wrote, gpio_s_length - wrote);
        if (wrote == -1) {
            perror(NULL);
            exit(1);
        }
    }
}
void export_gpio(int gpio)
{
    export_unexport_gpio(gpio, export_fd);

    int direction_fd;
    char direction_file_name[sizeof("gpio00/direction")];
    int direction_file_name_length;
    direction_file_name_length = snprintf(direction_file_name, sizeof(direction_file_name), "gpio%d/direction", gpio);
    printf("%s\n", direction_file_name);
}

void unexport_gpio(int gpio)
{
    export_unexport_gpio(gpio, unexport_fd);
}

void initialize()
{
    if (chdir("/sys/class/gpio/")==-1) {
        perror(NULL);
        exit(1);
    }

    export_fd = open("export", O_WRONLY, O_TRUNC);
    if (export_fd == -1) {
        perror(NULL);
        exit(1);
    }

    int i;
    for (i = 0; ALL_SEGMENTS[i] != 0; i++) {
        int gpio = ALL_SEGMENTS[i];
        export_gpio(gpio);
    }

    if (close(export_fd) == -1) {
        perror(NULL);
    }
}

void clean_up()
{
    unexport_fd = open("unexport", O_WRONLY, O_TRUNC);
    if (unexport_fd == -1) {
        perror(NULL);
        exit(1);
    }
    int i;
    for (i = 0; ALL_SEGMENTS[i] != 0; i++) {
        unexport_gpio(ALL_SEGMENTS[i]);
    }

    if (close(unexport_fd) == -1) {
        perror(NULL);
    }
}

void turn_segement_on_off(int segment, int power)
{
}

int main(int argc, char** argv)
{
    initialize();
    clean_up();
    return 0;
}
