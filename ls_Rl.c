/* See the LICENSE file for copyright and license details. */

/* for alphasort */
#define _DEFAULT_SOURCE

#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Error codes. */
enum {
        WRONG_USAGE = -2
};

#define PATHNAME_LENGTH_MAX 1024

#define PRINT_USAGE_HINT() printf("Usage: %s [-d pathname].\n", argv[0])

int
main(int argc, char *argv[])
{
        char pathname[PATHNAME_LENGTH_MAX];
        int nentries; /* A number of directory entries. */
        struct dirent **namelist;

        { /* Parse options. */
                int opt;
                bool d_found;

                d_found = false;
                while ((opt = getopt(argc, argv, "d:")) != -1) {
                        switch (opt) {
                        case 'd':
                                strcpy(pathname, optarg);
                                d_found = true;
                                break;
                        default:
                                PRINT_USAGE_HINT();
                                exit(WRONG_USAGE);
                                break;
                        }
                }
                if (!d_found) {
                        strcpy(pathname, ".");
                }
        }

        if ((nentries = scandir(pathname, &namelist, NULL, alphasort)) < 0) {
                perror("scandir");
                exit(errno);
        }
        for (int i = 0; i < nentries; ++i) {
                if (namelist[i]->d_name[0] != '.')
                        printf("%s\n", namelist[i]->d_name);
                free(namelist[i]);
        }
        free(namelist);

        exit(EXIT_SUCCESS);
}
