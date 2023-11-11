void write_in_output_file(int output_file, char *result)
{
    char s;
    int scrietot = 0;
    while ((s = result[scrietot]) != '\0')
    {
        scrietot++;
    }
    if ((write(output_file, result, scrietot)) < 0)
    {
        perror("Nu putem scrie in fisier");
        exit(1);
    }
}

void directory_entry(int output_file, char *name, struct stat stats)
{
    char result[BUFF_SIZE];
    sprintf(result, "nume director: %s\n"
                    "identificatorul utilizatorului: %d\n"
                    "drepturi de acces useri: %s\n"
                    "drepturi de acces grup: %s\n"
                    "drepturi de acces altii: %s\n\n",
            name, stats.st_uid, user_rights_checker(stats), group_rights_checker(stats), other_rights_checker(stats));

    write_in_output_file(output_file, result);
}

void file_entry(int output_file, char *name, struct stat stats)
{
    char result[BUFF_SIZE];
    char timeToString[11];

    struct tm *lastModTime = localtime(&stats.st_mtime);
    strftime(timeToString, 11, "%d.%m.%Y", lastModTime);

    sprintf(result, "nume fisier: %s\n"
                    "identificatorul utilizatorului: %d\n"
                    "dimensiunea fisierului: %ld\n"
                    "timpul ultimei modificari: %s\n"
                    "contorul de legaturi: %ld\n"
                    "drepturi de acces useri: %s\n"
                    "drepturi de acces grup: %s\n"
                    "drepturi de acces altii: %s\n\n",
            name, stats.st_uid, stats.st_size,
            timeToString, stats.st_nlink, user_rights_checker(stats), group_rights_checker(stats), other_rights_checker(stats));
    write_in_output_file(output_file, result);
}

void link_entry(char *new_path, int output_file, char *name, struct stat stats)
{
    struct stat ln_stats;
    lstat(new_path, &ln_stats);

    char result[BUFF_SIZE];
    sprintf(result, "nume legatura: %s\n"
                    "dimensiune legatura: %ld\n"
                    "dimensiune fisier: %ld\n"
                    "drepturi de acces useri: %s\n"
                    "drepturi de acces grup: %s\n"
                    "drepturi de acces altii: %s\n\n",
            name, ln_stats.st_size, stats.st_size, user_rights_checker(stats), group_rights_checker(stats), other_rights_checker(stats));

    write_in_output_file(output_file, result);
}

bmp_header bmp_header_reader(char *bmp_file)
{
    int fd = open(bmp_file, O_RDONLY);
    if (fd == -1)
    {
        perror("Nu s-a putut deschide fișierul");
        exit(EXIT_FAILURE);
    }

    bmp_header bmp_file_header;
    // file size
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
    // width
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
    // height
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
    // image_size
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

    close(fd);
    return bmp_file_header;
}

void bmp_file_entry(char *bmp_file, int output_file, char *name, struct stat stats)
{
    bmp_header bmp_file_header = bmp_header_reader(bmp_file);
    char result[BUFF_SIZE];
    char timeToString[11];

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
            timeToString, stats.st_nlink, user_rights_checker(stats), group_rights_checker(stats), other_rights_checker(stats));
    write_in_output_file(output_file, result);
}