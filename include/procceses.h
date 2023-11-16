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