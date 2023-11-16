/**
 * @brief in principiu aceasta functie iti va citi cate caractere trebuie sa scrie in fisier dupa care va apela functia de write si va scrie. Rolul ei este de a nu avea artefacte in fisierul de iesire ulterior
 *
 * @param out_dir folderul de output
 * @param filename numele fisierului de statistica in care se va scrie
 * @param result sirul de caractere care trebuie scris
 */
void write_in_output_file(const char *out_dir, const char *filename, char *result)
{
    char filepath[PATH_MAX];
    snprintf(filepath, sizeof(filepath), "%s/%s", out_dir, filename);

    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1)
    {
        perror("Eroare la deschiderea fisierului");
        exit(1);
    }

    char s;
    int scrietot = 0;
    // citesc caracter cu caracter si incrementez contorul
    while ((s = result[scrietot]) != '\0')
    {
        scrietot++;
    }
    // scriu in fisier exact cat trebuie ca sa nu am artefacte in fisier
    if ((write(fd, result, scrietot)) < 0)
    {
        perror("Nu putem scrie in fisier");
        exit(1);
    }

    close(fd);
}

/**
 * @brief Aceasta functie are rolul de a face jumpurile necesare in header ca sa obtina datele de care are nevoie pentru a le transmite mai departe
 *
 * @param bmp_file fisierul bmp pe care l am intalnit
 * @return bmp_header structul cu datele necesare, citite din header
 */
bmp_header bmp_header_reader(char *bmp_file)
{
    // deschidem fisierul ca sa citim headerul
    int fd = open(bmp_file, O_RDONLY);
    if (fd == -1)
    {
        perror("Nu s-a putut deschide fișierul");
        exit(EXIT_FAILURE);
    }

    bmp_header bmp_file_header;

    // Citim informatiile despre file size si le scriem in struct
    if (lseek(fd, file_size, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &bmp_file_header.fileSize, sizeof(bmp_file_header.fileSize)) == -1)
    {
        perror("Eroare la citirea din fișier");
        exit(EXIT_FAILURE);
    }

    // Citim informatiile despre width si le scriem in struct
    if (lseek(fd, width_offset, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &bmp_file_header.width, sizeof(bmp_file_header.width)) == -1)
    {
        perror("Eroare la citirea din fișier");
        exit(EXIT_FAILURE);
    }

    // Citim informatiile despre height si le scriem in struct
    if (lseek(fd, height_offset, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &bmp_file_header.height, sizeof(bmp_file_header.height)) == -1)
    {
        perror("Eroare la citirea din fișier");
        exit(EXIT_FAILURE);
    }

    // Citim informatiile despre sizeul imaginii si le scriem in struct
    if (lseek(fd, image_size, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &bmp_file_header.imageSize, sizeof(bmp_file_header.imageSize)) == -1)
    {
        perror("Eroare la citirea din fișier");
        exit(EXIT_FAILURE);
    }

    // Citim informatiile despre offset sa stim cat sarim
    if (lseek(fd, header_offset, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &bmp_file_header.headerOffset, sizeof(bmp_file_header.headerOffset)) == -1)
    {
        perror("Eroare la citirea din fișier");
        exit(EXIT_FAILURE);
    }

    // Citim informatiile despre bitcount sa stim cat sarim
    if (lseek(fd, bitcount_offset, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &bmp_file_header.bitCount, sizeof(bmp_file_header.bitCount)) == -1)
    {
        perror("Eroare la citirea din fișier");
        exit(EXIT_FAILURE);
    }

    // inchidem si returnam structul cu datele dorite
    close(fd);
    return bmp_file_header;
}

/**
 * @brief Functia are rolul de a seta detaliile cerute in enunt in cazul in care sa intalnit un entry de tip directory.
 *
 * @param output_file fisierul in care se scrie rezultatul
 * @param name numele entryului
 * @param stats este struct stat pentru entryul intalnit
 */
void directory_entry(const char *out_dir, char *name, struct stat stats)
{
    char output_filename[PATH_MAX];
    char result[BUFF_SIZE];
    char *user_rights = user_rights_checker(stats);
    char *group_rights = group_rights_checker(stats);
    char *other_rights = other_rights_checker(stats);
    sprintf(result, "nume director: %s\n"
                    "identificatorul utilizatorului: %d\n"
                    "drepturi de acces useri: %s\n"
                    "drepturi de acces grup: %s\n"
                    "drepturi de acces altii: %s\n\n",
            name, stats.st_uid, user_rights, group_rights, other_rights);

    snprintf(output_filename, sizeof(output_filename), "%s_statistica.txt", name);

    write_in_output_file(out_dir, output_filename, result);
    free(user_rights);
    free(group_rights);
    free(other_rights);
}

/**
 * @brief Functia are rolul de a seta detaliile cerute in enunt in cazul in care sa intalnit un entry de tip file care nu este BMP.
 *
 * @param output_file fisierul in care se scrie rezultatul
 * @param name numele entryului
 * @param stats este struct stat pentru entryul intalnit
 */
void file_entry(const char *out_dir, char *name, struct stat stats)
{
    char output_filename[PATH_MAX];
    char result[BUFF_SIZE];
    char timeToString[11];

    struct tm *lastModTime = localtime(&stats.st_mtime);
    strftime(timeToString, 11, "%d.%m.%Y", lastModTime);

    char *user_rights = user_rights_checker(stats);
    char *group_rights = group_rights_checker(stats);
    char *other_rights = other_rights_checker(stats);

    sprintf(result, "nume fisier: %s\n"
                    "identificatorul utilizatorului: %d\n"
                    "dimensiunea fisierului: %ld\n"
                    "timpul ultimei modificari: %s\n"
                    "contorul de legaturi: %ld\n"
                    "drepturi de acces useri: %s\n"
                    "drepturi de acces grup: %s\n"
                    "drepturi de acces altii: %s\n\n",
            name, stats.st_uid, stats.st_size,
            timeToString, stats.st_nlink, user_rights, group_rights, other_rights);

    snprintf(output_filename, sizeof(output_filename), "%s_statistica.txt", name);

    write_in_output_file(out_dir, output_filename, result);
    free(user_rights);
    free(group_rights);
    free(other_rights);
}

/**
 * @brief Functia are rolul de a seta detaliile cerute in enunt in cazul in care sa intalnit un entry de tip symbolic link.
 *
 * @param output_file fisierul in care se scrie rezultatul
 * @param name numele entryului
 * @param stats este struct stat pentru entryul intalnit
 */
void link_entry(char *new_path, const char *out_dir, char *name, struct stat stats)
{
    char output_filename[PATH_MAX];
    struct stat ln_stats;
    lstat(new_path, &ln_stats);

    char *user_rights = user_rights_checker(stats);
    char *group_rights = group_rights_checker(stats);
    char *other_rights = other_rights_checker(stats);

    char result[BUFF_SIZE];
    sprintf(result, "nume legatura: %s\n"
                    "dimensiune legatura: %ld\n"
                    "dimensiune fisier: %ld\n"
                    "drepturi de acces useri: %s\n"
                    "drepturi de acces grup: %s\n"
                    "drepturi de acces altii: %s\n\n",
            name, ln_stats.st_size, stats.st_size, user_rights, group_rights, other_rights);

    snprintf(output_filename, sizeof(output_filename), "%s_statistica.txt", name);

    write_in_output_file(out_dir, output_filename, result);
    free(user_rights);
    free(group_rights);
    free(other_rights);
}

/**
 * @brief Functia are rolul de a seta detaliile cerute in enunt in cazul in care s a intalnit un entry de tip file care este BMP.
 *
 * @param output_file fisierul in care se scrie rezultatul
 * @param name numele entryului
 * @param stats este struct stat pentru entryul intalnit
 */
void bmp_file_entry(char *bmp_file, const char *out_dir, char *name, struct stat stats)
{
    bmp_header bmp_file_header = bmp_header_reader(bmp_file);
    char output_filename[PATH_MAX];
    char result[BUFF_SIZE];
    char timeToString[11];

    char *user_rights = user_rights_checker(stats);
    char *group_rights = group_rights_checker(stats);
    char *other_rights = other_rights_checker(stats);

    struct tm *lastModTime = localtime(&stats.st_mtime);
    strftime(timeToString, 11, "%d.%m.%Y", lastModTime);

    sprintf(result, "nume fisier: %s\n"
                    "inaltime: %u\n"
                    "lungime: %u\n"
                    "identificatorul utilizatorului: %d\n"
                    "dimensiunea fisierului: %d\n"
                    "timpul ultimei modificari: %s\n"
                    "contorul de legaturi: %ld\n"
                    "drepturi de acces useri: %s\n"
                    "drepturi de acces grup: %s\n"
                    "drepturi de acces altii: %s\n\n",
            name, bmp_file_header.height, bmp_file_header.width, stats.st_uid, bmp_file_header.fileSize,
            timeToString, stats.st_nlink, user_rights, group_rights, other_rights);

    snprintf(output_filename, sizeof(output_filename), "%s_statistica.txt", name);

    write_in_output_file(out_dir, output_filename, result);
    free(user_rights);
    free(group_rights);
    free(other_rights);
}
