/**
 * @brief Aceasta functie are rolul de a face grayscale la o imagine BMP pe 8 biti.
 * Ce se intampla in acest caz este ca functia mea va trebui sa modifice defapt paleta de culori gasita imediat dupa headerul fisierului BMP.
 *
 * @param fd file descriptorul fisierul BMP
 */
void proc_8bit_bmp(int fd)
{
    // sar la zona unde se afla paleta de culori
    if (lseek(fd, offsetToPalette, SEEK_SET) == -1)
    {
        perror("Eroare la cautrarea in fisier");
        exit(EXIT_FAILURE);
    }

    // stiind ca este pe 8 biti voi avea exact 256 de culori disponibile
    for (int i = 0; i < 256; ++i)
    {
        // citim bitii de culoare
        unsigned char color[4];
        if (read(fd, color, 4) == -1)
        {
            perror("Eroare la citirea datelor pixelilor");
            break;
        }
        // scalam si punem la loc noua proportie
        unsigned char gray = color[0] * 0.299 + color[1] * 0.587 + color[2] * 0.114;
        color[0] = color[1] = color[2] = gray;

        // din cauza citirii mutam inapoi pointerul cu cat am citit
        lseek(fd, -4, SEEK_CUR);

        // scriem peste noua culoare gri a pixelilor
        if (write(fd, color, 4) == -1)
        {
            perror("Eroare la scrierea datelor pixelilor");
            break;
        }
    }
}
/**
 * @brief  Aceasta functie are rolul de a face grayscale la o imagine BMP pe 24 biti.
 * Ce se intampla aici defapt, este ca va trebui sa sar la locul din care incep cu adevarat pixelii pentru a incepe sa ii modific.
 * Pentru performanta voi citi chunk uri de date care sunt exact cat latimea imaginii.
 * Practic voi procesa pe rand cate un rand din aceasta imagine pentru a avea un proces mai rapid de citire/scriere.
 *
 * @param fd file descriptorul fisierul BMP
 * @param bmp_file_header headerul specific fisierului BMP
 */
void proc_24bit_bmp(int fd, bmp_header bmp_file_header)
{
    // sarim la pozitia de unde incep pixelii si incepem sa modificam
    if (lseek(fd, bmp_file_header.headerOffset, SEEK_SET) == -1)
    {
        perror("Eroare la cautarea in fisier");
        exit(EXIT_FAILURE);
    }

    // ne alocam memorie pentru fiecare rand de date ce trebuie citit.
    size_t blockSize = bmp_file_header.width * 3;
    unsigned char *buffer = malloc(blockSize);
    if (buffer == NULL)
    {
        perror("Eroare la alocarea memoriei");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // parcurcem ficare rand din imagine
    for (int i = 0; i < bmp_file_header.height; i++)
    {
        // citim o intreaga lungime a imaginii
        if (read(fd, buffer, blockSize) == -1)
        {
            perror("Eroare la citirea datelor pixelilor");
            break;
        }

        // procesez pe rand fiecare pixel
        for (int j = 0; j < blockSize; j += 3)
        {
            unsigned char aux = buffer[j] * 0.299 + buffer[j + 1] * 0.587 + buffer[j + 2] * 0.114;
            buffer[j] = buffer[j + 1] = buffer[j + 2] = aux;
        }

        // ma intorc la pozitia de unde am citit
        lseek(fd, -blockSize, SEEK_CUR);

        // scriu la loc lungimea citita
        if (write(fd, buffer, blockSize) != blockSize)
        {
            perror("Eroare la scrierea datelor pixelilor");
            break;
        }
    }
    free(buffer);
}

/**
 * @brief Acesta functie are in principiu rolul de a decide ce tip de bmp este si sa execute secventa de cod pentru acel tip de BMP.
 *
 * @param bmp_file calea relativa catre fisierul BMP dorit spre procesare.
 */
void process_image(char *bmp_file)
{
    // citim doar datele necesare din headerul bmp
    bmp_header bmp_file_header = bmp_header_reader(bmp_file);

    // deschidem fisierul ca sa citim datele
    int fd = open(bmp_file, O_RDWR);
    if (fd == -1)
    {
        perror("Nu s-a putut deschide fisierul");
        exit(EXIT_FAILURE);
    }

    // in functie de bitCount voi decide modul in care voi scala gri imaginea
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