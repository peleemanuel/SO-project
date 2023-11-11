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

void bmp_header_reader()
{
}