
char *user_rights_checker(struct stat stats)
{
    char *user_rights = malloc(4 * sizeof(char));
    if (user_rights == NULL)
    {
        perror("Insufficient memory");
        exit(EXIT_FAILURE);
    }
    user_rights[0] = (stats.st_mode & S_IRUSR) ? 'R' : '-';
    user_rights[1] = (stats.st_mode & S_IWUSR) ? 'W' : '-';
    user_rights[2] = (stats.st_mode & S_IXUSR) ? 'X' : '-';
    user_rights[3] = '\0';
    return user_rights;
}

char *group_rights_checker(struct stat stats)
{
    char *group_rights = malloc(4 * sizeof(char));
    if (group_rights == NULL)
    {
        perror("Insufficient memory");
        exit(EXIT_FAILURE);
    }
    group_rights[0] = (stats.st_mode & S_IRGRP) ? 'R' : '-';
    group_rights[1] = (stats.st_mode & S_IWGRP) ? 'W' : '-';
    group_rights[2] = (stats.st_mode & S_IXGRP) ? 'X' : '-';
    group_rights[3] = '\0';
    return group_rights;
}

char *other_rights_checker(struct stat stats)
{
    char *other_rights = malloc(4 * sizeof(char));
    if (other_rights == NULL)
    {
        perror("Insufficient memory");
        exit(EXIT_FAILURE);
    }
    other_rights[0] = (stats.st_mode & S_IROTH) ? 'R' : '-';
    other_rights[1] = (stats.st_mode & S_IWOTH) ? 'W' : '-';
    other_rights[2] = (stats.st_mode & S_IXOTH) ? 'X' : '-';
    other_rights[3] = '\0';
    return other_rights;
}
