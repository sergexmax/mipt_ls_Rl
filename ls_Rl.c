/* For alphasort. */
#define _DEFAULT_SOURCE

#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Error codes. */
enum {
        WRONG_USAGE = -2
};

#define PATHNAME_LENGTH_MAX 1024
#define NAME_MAX_LENGTH 1024

#define PRINT_USAGE_HINT() printf("Usage: %s [-d pathname].\n", argv[0])

/* Print type, permissions. */
void
print_mode(mode_t mode)
{
        char type;

        type = '?';
        if (S_ISREG(mode))
                type = '-';
        if (S_ISDIR(mode))
                type = 'd';
        if (S_ISLNK(mode))
                type = 'l';
        if (S_ISBLK(mode))
                type = 'b';
        if (S_ISCHR(mode))
                type = 'c';
        if (S_ISFIFO(mode))
                type = '|';
        printf("%c", type);
        printf("%c%c%c%c%c%c%c%c%c ",
                mode & S_IRUSR ? 'r' : '-',
                mode & S_IWUSR ? 'w' : '-',
                mode & S_IXUSR ? 'x' : '-',
                mode & S_IRGRP ? 'r' : '-',
                mode & S_IWGRP ? 'w' : '-',
                mode & S_IXGRP ? 'x' : '-',
                mode & S_IROTH ? 'r' : '-',
                mode & S_IWOTH ? 'w' : '-',
                mode & S_IXOTH ? 'x' : '-');
}

/* Print entry information such as type, permissions and etc. */
void
print_entry_info(char *pathname)
{
        struct stat statbuf;
        struct passwd pwd;
        struct passwd *pwd_res;
        struct group grp;
        struct group *grp_res;
        char buf[NAME_MAX_LENGTH];
        struct tm time;

        if (stat(pathname, &statbuf)) {
                perror("stat");
                exit(errno);
        }
        print_mode(statbuf.st_mode);
        printf("%lu ", statbuf.st_nlink);
        getpwuid_r(statbuf.st_uid, &pwd, buf, sizeof(buf), &pwd_res);
        if (pwd_res == NULL) {
                perror("getpwuid_r");
                exit(errno);
        }
        printf("%s ", pwd.pw_name);
        getgrgid_r(statbuf.st_gid, &grp, buf, sizeof(buf), &grp_res);
        if (grp_res == NULL) {
                perror("getgrgid_r");
                exit(errno);
        }
        printf("%s ", grp.gr_name);
        printf("%5ld ", statbuf.st_size);
        if (localtime_r(&statbuf.st_mtime, &time) == NULL) {
                perror("localtime_r");
                exit(errno);
        }
        if (strftime(buf, sizeof(buf), "%b %d %H:%M", &time) == 0) {
                perror("strftime");
                exit(errno);
        }
        printf("%s ", buf);

}

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
                if (namelist[i]->d_name[0] != '.') {
                        sprintf(entry_pathname, "%s%s", pathname, namelist[i]->d_name);
                        print_entry_info(entry_pathname);
                        printf("%s\n", namelist[i]->d_name);
                }
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
