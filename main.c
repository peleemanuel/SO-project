#include "include/includes.h"
#include "include/rights_checker.h"
#include "include/dir_entry_proc.h"

bool is_bmp(const char *file_path)
{
    const char *dot = strrchr(file_path, '.');
    if (!dot || dot == file_path)
        return false;
    bool result = strcmp(dot, ".bmp") == 0;
    return result;
}

void parse_directory(int output_file, const char *path)
{

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("Unable to open directory");
        return;
    }

    struct dirent *entry;
    char new_path[PATH_MAX];

    while ((entry = readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);

        struct stat dir_entry;
        stat(new_path, &dir_entry);

        if (entry->d_type == DT_REG)
        {
            if (!is_bmp(entry->d_name))
            {
                printf("File: %s\n", new_path);
                file_entry(output_file, entry->d_name, dir_entry);
            }
            else
            {
                printf("AM GASIT UN BMP SIUUU\n");
            }
        }

        else if (entry->d_type == DT_DIR)
        {
            printf("Directory: %s\n", new_path);
            directory_entry(output_file, entry->d_name, dir_entry);
            parse_directory(output_file, new_path);
        }
        else if (entry->d_type == DT_LNK)
        {
            printf("Link: %s\n", new_path);
            link_entry(new_path, output_file, entry->d_name, dir_entry);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <path_to_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat info;
    stat(argv[1], &info);
    if (!S_ISDIR(info.st_mode))
    {
        printf("Argument %s must be a directory\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fdOUT;
    // creem fisierul in care vom scrie datele despre fiecare fisier:
    fdOUT = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fdOUT == -1)
    {
        perror("Nu putem crea fisierul");
        exit(1);
    }

    parse_directory(fdOUT, argv[1]);
    // inchidem fisierul in care scriem
    if (close(fdOUT) < 0)
    {
        perror("Eroare la inchiderea fisierului de output");
        exit(-1);
    }

    return 0;
}
