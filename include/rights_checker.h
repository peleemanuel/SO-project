/**
 * @brief Functie ce verifica drepturile pe care le are USERul pentru entryul ce este dorit sa fie verificat
 *
 * @param stats este structul stat pentru fisierul pe care se doreste verificarea drepturilor
 * @return char* cu campurile posibile pentru drepturi cu litera mare sau cu - in absenta lor
 */
char *user_rights_checker(struct stat stats)
{
    // alocam memorie pentru a face scrierea unui char cu drepturile
    char *user_rights = malloc(4 * sizeof(char));
    if (user_rights == NULL)
    {
        perror("Insufficient memory");
        exit(EXIT_FAILURE);
    }

    // verificam drepturile cu operatorii ternari
    user_rights[0] = (stats.st_mode & S_IRUSR) ? 'R' : '-';
    user_rights[1] = (stats.st_mode & S_IWUSR) ? 'W' : '-';
    user_rights[2] = (stats.st_mode & S_IXUSR) ? 'X' : '-';
    user_rights[3] = '\0';
    return user_rights;
}

/**
 * @brief Functie ce verifica drepturile pe care le are GROUPul pentru entryul ce este dorit sa fie verificat
 *
 * @param stats este structul stat pentru fisierul pe care se doreste verificarea drepturilor
 * @return char* cu campurile posibile pentru drepturi cu litera mare sau cu - in absenta lor
 */
char *group_rights_checker(struct stat stats)
{
    // alocam memorie pentru a face scrierea unui char cu drepturile
    char *group_rights = malloc(4 * sizeof(char));
    if (group_rights == NULL)
    {
        perror("Insufficient memory");
        exit(EXIT_FAILURE);
    }

    // verificam drepturile cu operatorii ternari
    group_rights[0] = (stats.st_mode & S_IRGRP) ? 'R' : '-';
    group_rights[1] = (stats.st_mode & S_IWGRP) ? 'W' : '-';
    group_rights[2] = (stats.st_mode & S_IXGRP) ? 'X' : '-';
    group_rights[3] = '\0';
    return group_rights;
}

/**
 * @brief Functie ce verifica drepturile pe care le are OTHERS pentru entryul ce este dorit sa fie verificat
 *
 * @param stats este structul stat pentru fisierul pe care se doreste verificarea drepturilor
 * @return char* cu campurile posibile pentru drepturi cu litera mare sau cu - in absenta lor
 */
char *other_rights_checker(struct stat stats)
{
    // alocam memorie pentru a face scrierea unui char cu drepturile
    char *other_rights = malloc(4 * sizeof(char));
    if (other_rights == NULL)
    {
        perror("Insufficient memory");
        exit(EXIT_FAILURE);
    }

    // verificam drepturile cu operatorii ternari
    other_rights[0] = (stats.st_mode & S_IROTH) ? 'R' : '-';
    other_rights[1] = (stats.st_mode & S_IWOTH) ? 'W' : '-';
    other_rights[2] = (stats.st_mode & S_IXOTH) ? 'X' : '-';
    other_rights[3] = '\0';
    return other_rights;
}

/**
 * @brief Functie care verifica daca am toate drepturile de citire pentru un anumit fisier/folder. Va fi folosit in principiu pentru verificarile initiale.
 *
 * @param stats structul stat al fisierului folderului ce trebuie testat.
 * @return true in caz ca avem toate drepturile de citire
 * @return false in caz contar
 */
bool read_rights_checker(struct stat stats)
{
    bool result = false;
    char *user_rights = user_rights_checker(stats);
    char *group_rights = group_rights_checker(stats);
    char *other_rights = other_rights_checker(stats);

    if (user_rights[0] == 'R' && group_rights[0] == 'R' && other_rights[0] == 'R')
        result = true;
    free(user_rights);
    free(group_rights);
    free(other_rights);

    return result;
}

/**
 * @brief Functie care verifica daca am toate drepturile de scriere pentru un anumit fisier/folder. Va fi folosit in principiu pentru verificarile initiale.
 *
 * @param stats structul stat al fisierului folderului ce trebuie testat.
 * @return true in caz ca avem toate drepturile de scriere
 * @return false in caz contar
 */
bool write_rights_checker(struct stat stats)
{
    bool result = false;
    char *user_rights = user_rights_checker(stats);
    char *group_rights = group_rights_checker(stats);
    char *other_rights = other_rights_checker(stats);

    if (user_rights[1] == 'W' && group_rights[1] == 'W' && other_rights[1] == 'W')
        result = true;
    free(user_rights);
    free(group_rights);
    free(other_rights);

    return result;
}