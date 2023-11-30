/**
 * @brief Aceasta functie are rolul de a rezolva cerinta B din Milostone ul saptamanii 9.
 * Aceasta creaza 2 pipeuri unidirectionale intre cei doi fii resprectiv tata-fiu pentru a transmite datele cerute.
 * Se foloseste comanda cat in primul fiu pentru a scrie datele din fisier mai departe prin pipe catre celalalt fiu.
 * Noul fiu proceseaza noile date cu scriptul srcript.sh care verifica corectitudinea propozitiei si valideaza existenta caracterului primit ca parametru.
 * Ulterior tatal afiseaza datele cerute la ecran.
 *
 * @param out_dir Reprezinta folderul in care se va scrie fisierul de statistica
 * @param entry structul de directory entry
 * @param dir_entry structul stat al intrarii
 * @param new_path calea relativa a fisierului
 * @param c caracterul c ce trebuie validat in propozitie
 */
void piping_regular_file_processes(const char *out_dir, struct dirent *entry, struct stat dir_entry, const char *new_path, const char *c)
{
    // FF - pipe fiu-fiu
    // TF - pipe tata-fiu
    int FF[2], TF[2];
    pid_t pid1, pid2;

    // Crearea unui pipe
    if (pipe(TF) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Crearea unui pipe
    if (pipe(FF) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // pornim primul proces fiu
    pid1 = fork();
    if (pid1 == -1)
    {
        perror("Error in fork");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0)
    {
        // inchidem canalele nenecesare
        close(TF[0]);
        close(TF[1]);
        close(FF[0]);

        // procesam fisierul de statistica in mod normal
        file_entry(out_dir, entry->d_name, dir_entry);

        // redirectionam stdout pe pipe ul catre al doilea fiu
        if ((dup2(FF[1], 1)) < 0)
        {
            perror("Eroare la dup2\n");
            exit(EXIT_FAILURE);
        };
        close(FF[1]);

        // Ca sa putem face o comanda ce sa dea exit cu numarul de linii scrise in fisierul de exit, o sa ii facem o comanda care sa dea exit cu acest numar pentru ca nu mai vom putea controla odata ce se intra in execlp
        char command[PATH_MAX];
        sprintf(command, "cat %s ; exit %d", new_path, 8);

        // folosim comanda cat pentru a afisa continutul care defapt este transmis catre urmatorul pipe ca intrare.
        execlp("bash", "bash", "-c", command, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // pornim cel de al doilea proces
    pid2 = fork();
    if (pid2 == -1)
    {
        perror("Error in fork");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0)
    {
        // inchidem canalele nenecesare
        close(FF[1]);
        close(TF[0]);

        // voi citi acum intrarea de la procesul fiu1 si o voi folosi ca intrare pentru acest proces
        if ((dup2(FF[0], 0)) < 0)
        {
            perror("Eroare la dup2\n");
            exit(EXIT_FAILURE);
        };
        close(FF[0]);

        // ceea ce voi dori sa afisez voi trimite acum prin pipe catre procesul tata.
        if ((dup2(TF[1], 1)) < 0)
        {
            perror("Eroare la dup2\n");
            exit(EXIT_FAILURE);
        };
        close(TF[1]);

        // executam scriptul bash cu propozitiile ce vin din celalalt proces si le verificam conform scriptului
        execlp("bash", "bash", "script.sh", c, (char *)NULL);
        perror("execlp 2");
        exit(EXIT_FAILURE);
    }

    // ma aflu in procesul tata
    // inchidem canalele nenecesare
    close(FF[0]);
    close(FF[1]);
    close(TF[1]);

    // astept ca procesul fiu care citeste sa se termine ca sa ma asigur ca si au incheiat activitatea de transmitere iar pe canalul tata fiu voi avea sigur rezultatul asteptat.
    waitpid(pid2, NULL, 0);

    // citim rezultatul
    char buffer[BUFF_SIZE];
    read(TF[0], buffer, BUFF_SIZE);
    close(TF[0]);

    // afisam rezultatul convertit
    printf("Numarul de propozitii corespunzatoare: %d\n", atoi(buffer));
}
/**
 * @brief In principiu aceasta functie are simplul rol de a inchide procesele cu mesajul cerut in MILESTONE S8.
 *
 */
void wait_for_all_child_processes()
{
    int pid;
    int status;

    while ((pid = wait(&status)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("Procesul copil cu PID %d s-a terminat cu statusul %d\n", pid, WEXITSTATUS(status));
        }
    }
}
