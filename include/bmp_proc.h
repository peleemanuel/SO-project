void proc_8bit_bmp(int fd)
{
    if (lseek(fd, offsetToPalette, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 256; ++i)
    {
        unsigned char color[4];
        if (read(fd, color, 4) == -1)
        {
            perror("Eroare la citirea datelor pixelilor");
            break;
        }
        unsigned char gray = color[0] * 0.299 + color[1] * 0.587 + color[2] * 0.114;
        color[0] = color[1] = color[2] = gray;

        lseek(fd, -4, SEEK_CUR);
        if (write(fd, color, 4) == -1)
        {
            perror("Eroare la scrierea datelor pixelilor");
            break;
        }
    }
}

void proc_24bit_bmp(int fd, bmp_header bmp_file_header)
{
    // sarim la pozitia de unde incep pixelii si incepem sa modificam
    if (lseek(fd, bmp_file_header.headerOffset, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fișier");
        exit(EXIT_FAILURE);
    }

    size_t blockSize = bmp_file_header.width * 3;
    unsigned char *buffer = malloc(blockSize);
    if (buffer == NULL)
    {
        perror("Eroare la alocarea memoriei");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < bmp_file_header.height; i++)
    {
        if (read(fd, buffer, blockSize) == -1)
        {
            perror("Eroare la citirea datelor pixelilor");
            break;
        }

        for (int j = 0; j < blockSize; j += 3)
        {
            unsigned char aux = buffer[j] * 0.299 + buffer[j + 1] * 0.587 + buffer[j + 2] * 0.114;
            buffer[j] = buffer[j + 1] = buffer[j + 2] = aux;
        }

        lseek(fd, -blockSize, SEEK_CUR);
        if (write(fd, buffer, blockSize) != blockSize)
        {
            perror("Eroare la scrierea datelor pixelilor");
            break;
        }
    }
    free(buffer);
}

void process_image(char *bmp_file)
{
    bmp_header bmp_file_header = bmp_header_reader(bmp_file);

    // deschidem fisierul ca sa citim headerul
    int fd = open(bmp_file, O_RDWR);
    if (fd == -1)
    {
        perror("Nu s-a putut deschide fișierul");
        exit(EXIT_FAILURE);
    }

    if (bmp_file_header.bitCount == 24)
    {
        proc_24bit_bmp(fd, bmp_file_header);
    }
    else if (bmp_file_header.bitCount == 8)
    {
        proc_8bit_bmp(fd);
    }
    else
    {
        printf("Nu cunoastem acest format inca\n");
    }
    close(fd);
}