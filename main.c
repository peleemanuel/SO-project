#include "include/includes.h"
#include "include/rights_checker.h"
#include "include/dir_entry_proc.h"

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
 * @brief Functia parse_directory in principiu se asigura ca face apelarea pe toate intrarile de fisier si director in mod recursiv din directorul primit ca paramentru
 *
 * @param output_file este identificatorul fisierului in care scriu datele
 * @param path pathul folderului in care vreau sa fac apelul
 */
void parse_directory(int output_file, const char *path)
{
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
            printf("File: %s\n", new_path);
            if (!is_bmp(entry->d_name))
            {
                file_entry(output_file, entry->d_name, dir_entry);
            }
            else
            {
                bmp_file_entry(new_path, output_file, entry->d_name, dir_entry);
            }
        }

        // in cazul in care este director vom afisa cele cerute si pe langa asta vom apela recursiv functia pentru noul director intalnit
        else if (entry->d_type == DT_DIR)
        {
            printf("Directory: %s\n", new_path);
            directory_entry(output_file, entry->d_name, dir_entry);
            parse_directory(output_file, new_path);
        }

        // daca avem o legatura simbolica vom apela o functie ce rezolva cerinta ceruta
        else if (entry->d_type == DT_LNK)
        {
            printf("Link: %s\n", new_path);
            link_entry(new_path, output_file, entry->d_name, dir_entry);
        }
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
    if (argc != 2)
    {
        printf("Usage: %s <path_to_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // verific ca ceea ce am primit ca argument este un folder
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

    // apelam functia de parsare a fisierelor din directorul primit ca parametru
    parse_directory(fdOUT, argv[1]);

    // inchidem fisierul in care scriem
    if (close(fdOUT) < 0)
    {
        perror("Eroare la inchiderea fisierului de output");
        exit(-1);
    }

    return 0;
}
