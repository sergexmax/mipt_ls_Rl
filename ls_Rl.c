/* See the LICENSE file for copyright and license details. */

/* For alphasort. */
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

/* List entries.*/
void
ls_entries(char *pathname)
{
        int nentries; /* A number of directory entries. */
        struct dirent **namelist;
        char entry_pathname[PATHNAME_LENGTH_MAX];

        printf("%s:\n", pathname);
        if ((nentries = scandir(pathname, &namelist, NULL, alphasort)) < 0) {
                perror("scandir");
                exit(errno);
        }
        for (int i = 0; i < nentries; ++i) {
                if (namelist[i]->d_name[0] != '.')
                        printf("%s\n", namelist[i]->d_name);
        }
        for (int i = 0; i < nentries; ++i) {
                if (namelist[i]->d_type == DT_DIR && namelist[i]->d_name[0] != '.') {
                        printf("\n");
                        sprintf(entry_pathname, "%s%s/", pathname, namelist[i]->d_name);
                        ls_entries(entry_pathname);
                }
                free(namelist[i]);

        }
        free(namelist);
}

int
main(int argc, char *argv[])
{
        char pathname[PATHNAME_LENGTH_MAX];

        { /* Parse options. */
                int opt;
                bool d_found;

                d_found = false;
                while ((opt = getopt(argc, argv, "d:")) != -1) {
                        switch (opt) {
                        case 'd':
                                strcpy(pathname, optarg);
                                if (pathname[strlen(pathname) - 1] != '/')
                                        strcat(pathname, "/");
                                d_found = true;
                                break;
                        default:
                                PRINT_USAGE_HINT();
                                exit(WRONG_USAGE);
                                break;
                        }
                }
                if (!d_found)
                        strcpy(pathname, "./");
        }

        ls_entries(pathname);

        exit(EXIT_SUCCESS);
}
