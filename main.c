#include "include/includes.h"
#include "include/rights_checker.h"
#include "include/dir_entry_proc.h"
#include "include/procceses.h"
#include "include/bmp_proc.h"
/**
 * @brief Functia are ca rol sa determine daca fisierul este un bmp bazat pe extensia lui
 *
 * @param file_path calea relativa a fisierului ce vrem sa fie testat
 */
bool is_bmp(const char *file_path)
{
    // cauta primul punct de la coada la cap ca sa identifice extensia
    const char *dot = strrchr(file_path, '.');

    // in caz ca nu gaseste vreun punct returneaza fals
    if (!dot || dot == file_path)
        return false;

    // determina daca avem sau nu .bmp in coada
    bool result = strcmp(dot, ".bmp") == 0;
    return result;
}

/**
 * @brief Functia parse_directory in principiu se asigura ca face apelarea pe toate intrarile de fisier si director in mod recursiv din directorul primit ca parametru realizand toate operatiile cerute mai jos
 *
 * @param path este calea folderului din care citesc informatiile
 * @param out_dir este calea catre folderul in care se vor scrie fisierele de statistica
 * @param c reprezinta sirul de litere cu caracterul alfanumeric ce trebuie cautat in propozitii.
 */
void parse_directory(const char *path, const char *out_dir, const char *c)
{
    int pid;
    // deschidem fisierul pe care l am primit
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("Unable to open directory");
        return;
    }

    struct dirent *entry;
    char new_path[PATH_MAX];

    // cat timp avem entryuri in director le luam pe rand la citit
    while ((entry = readdir(dir)) != NULL)
    {
        // dam skip la entryurile de . si ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // facem calea relativa in functie de locatia apelului unde s a facut apelul functiei
        snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);

        // extragem cu stat datele despre fiecare intrare in folder
        struct stat dir_entry;
        stat(new_path, &dir_entry);

        // in cazul in care avem un fisier normal vom verifica daca acesta este bmp si in functie de asta vom decide ce vom alege sa facem in continuare
        if (entry->d_type == DT_REG)
        {
            if (!is_bmp(entry->d_name))
            {
                printf("File: %s\n", new_path);

                // rulam tot ceea ce se cere pentru un fisier normal aici
                piping_regular_file_processes(out_dir, entry, dir_entry, new_path, c);
            }
            else
            {
                printf("BMP: %s\n", new_path);
                pid = fork();
                if (pid == -1)
                {
                    perror("Error in fork");
                    exit(EXIT_FAILURE);
                }
                if (pid == 0)
                {
                    bmp_file_entry(new_path, out_dir, entry->d_name, dir_entry);
                    exit(10);
                }
                pid = fork();
                if (pid == -1)
                {
                    perror("Error in fork");
                    exit(EXIT_FAILURE);
                }
                if (pid == 0)
                {
                    process_image(new_path);
                    exit(0);
                }
            }
        }

        // in cazul in care este director vom afisa cele cerute si pe langa asta vom apela recursiv functia pentru noul director intalnit
        else if (entry->d_type == DT_DIR)
        {
            printf("Directory: %s\n", new_path);
            pid = fork();
            if (pid == -1)
            {
                perror("Error in fork");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                directory_entry(out_dir, entry->d_name, dir_entry);
                exit(5);
            }
            parse_directory(new_path, out_dir, c);
        }

        // daca avem o legatura simbolica vom apela o functie ce rezolva cerinta ceruta
        else if (entry->d_type == DT_LNK)
        {
            printf("Link: %s\n", new_path);
            pid = fork();
            if (pid == -1)
            {
                perror("Error in fork");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                link_entry(new_path, out_dir, entry->d_name, dir_entry);
                exit(6);
            }
        }
        sleep(1);
    }

    closedir(dir);
}

/**
 * @brief Functia de main care se apeleaza la execturea programului
 *
 * @param argc numarul de argumente ce il primeste ca parametru
 * @param argv argumentele primite ca parametru
 */
int main(int argc, char *argv[])
{
    // verific practic daca am numarul corespunzator de argumente
    if (argc != 4)
    {
        printf("Usage: %s <path_to_directory> <path_to_new_directory> <c>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // verific ca ceea ce am primit ca argument este un folder
    struct stat info;
    stat(argv[1], &info);
    if (!S_ISDIR(info.st_mode))
    {
        printf("First argument %s must be a directory\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // verific ca am drepturi de citire asupra folderului din care voi citii informatiile
    if (read_rights_checker(info))
        printf("We have all the read rights on this folder.\n");
    else
    {
        printf("We dont have all the read rights on this folder.\n");
        exit(EXIT_FAILURE);
    }

    stat(argv[2], &info);
    if (!S_ISDIR(info.st_mode))
    {
        printf("Second argument %s must be a directory\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    // verific ca am drepturi de scriere asupra folderului in care voi scrie informatiile
    if (write_rights_checker(info))
        printf("We have all the write rights on this folder.\n");
    else
    {
        printf("We dont have all the write rights on this folder.\n");
        exit(EXIT_FAILURE);
    }

    // verificam ca am primit un caracter aflanumeric la intratre
    if (strlen(argv[3]) != 1 || !isalnum(argv[3][0]))
    {
        printf("Argument 3 is not an alnum character\n");
        exit(EXIT_FAILURE);
    }

    // apelam functia de parsare a fisierelor din directorul primit ca parametru
    parse_directory(argv[1], argv[2], argv[3]);
    wait_for_all_child_processes();
    return 0;
}
