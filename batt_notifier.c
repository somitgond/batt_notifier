#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

int batt_perc(char *path) {
    FILE *file_ptr;

    // open file
    file_ptr = fopen(path, "r");

    if (NULL == file_ptr) {
        return -1;
    }

    int percentage;

    // read file
    if (fscanf(file_ptr, "%d", &percentage) != 1) {
        fclose(file_ptr);
        return -1;
    }
    fclose(file_ptr);
    return percentage;
}

// from here :
// https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
static void daemonize() {
    pid_t pid;

    // fork of parent process
    pid = fork();

    // An error occured
    if (pid < 0)
        exit(EXIT_FAILURE);

    // terminate parent process
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // On Success: Child process becomes session leader
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    // fork for second time
    // fork of parent process
    pid = fork();

    // An error occured
    if (pid < 0)
        exit(EXIT_FAILURE);

    // terminate parent process
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // set new file permissions
    umask(0);

    // change the working directory to temp
    chdir("/temp");

    // close all open file discriptors
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }
}

int main() {
    // daemonize the process
    daemonize();

    // notification state
    int notification_batt0 = 0;
    int notification_batt1 = 0;
    char command[100];

    while (1) {
        // get battery percentage
        int batt0_perc = batt_perc(BATT_PATH0);
        int batt1_perc = batt_perc(BATT_PATH1);

        // check condition for battery 0
        if (batt0_perc >= 0 && batt0_perc < THRESHOLD && !notification_batt0) {
            snprintf(command, sizeof(command),
                     "notify-send \"Battery 0 Below threshold: %d%%\"",
                     batt0_perc);
            system(command);
            notification_batt0 = 1;
        }

        // check for battery 1
        if (batt1_perc >= 0 && batt1_perc < THRESHOLD && !notification_batt1) {
            snprintf(command, sizeof(command),
                     "notify-send \"Battery 1 Below threshold: %d%%\"",
                     batt1_perc);
            system(command);
            notification_batt1 = 1;
        }

        // reset the notification state
        if (batt0_perc >= THRESHOLD) {
            notification_batt0 = 0;
        }

        if (batt1_perc >= THRESHOLD) {
            notification_batt1 = 0;
        }

        sleep(UPDATE_INTERVAL);
    }

    return 0;
}
