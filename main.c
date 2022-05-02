#include "gene.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "mpi.h"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int n = 1000;

unsigned int *alea_gen()
{
    unsigned int *gene = malloc(n * sizeof(int));

    srand(getpid());

    for (int i = 0; i < n; i++)
    {
        gene[i] = rand() % 2;
        // printf(" %d",gene[i]);
    }
    return gene;
}

int *test_gen()
{
    int *gene = calloc(sizeof(int *), MAX_GENES);

    // Example of a gene so the gene map register it
    gene[0] = 000011011100;

    return gene;
}

void print_test(gene_map_t *gene_map)
{
    printf("Début print de test\n");
    for (long long i = 0; i < gene_map->genes_counter; i++)
    {
        printf("Gene \x1b[31m%lld\x1b[0m: \x1b[32m[%lld:%lld]\x1b[0m\n", i, gene_map->gene_start[i], gene_map->gene_end[i]);
    }
}

/* Count files in a directory
 *
 */
int countfiles()
{
    int count = 0;
    struct dirent *entry;

    DIR *dir = opendir("./fastas/");

    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".fasta"))
            count++;
    }
    return count;
}

int readfiles(int size_r)
{

    int nb = countfiles();
    char **content = malloc(sizeof(char *) * nb);

    DIR *dir;

    FILE *input;

    // Open the directory which contain all the fastas files
    if ((dir = opendir("fastas")) == NULL)
        return printf("Error: Can't open fastas folder\n"), -1;

    struct dirent *file;

    int i = 0;

    // Iterate if a file exists in this directory
    while ((file = readdir(dir)) != NULL)
    {

        if (file->d_type == DT_DIR)
            continue;
        // Skip parent directory ( linux)
        if ((!strcmp(file->d_name, ".")) && (!strcmp(file->d_name, "..")))
            continue;

        char name[50] = "./fastas/";

        strcat(name, file->d_name);

        // Get size of the file to allocate enough memory
        struct stat st;
        stat(name, &st);
        long size = st.st_size;

        content[i] = (char *)malloc(size * sizeof(char));

        // Open fasta file
        if ((input = fopen(name, "r")) == NULL)
        {
            // fclose(input);
            return printf("Error: Can't open fastas file %s\n", name), -1;
        }
        printf("%s\n", file->d_name);

        char *line;
        size_t len = 0;
        ssize_t read;

        // first line , we need to skip it
        read = getline(&line, &len, input);

        content[i] = (char *)malloc((size - read - 1)* sizeof(char));
        getline(&line, &len, input);

        strcpy(content[i],line);

        while ((read = getline(&line, &len, input)) != -1)
        {
            // Toggle newline
            line[strcspn(line, "\n") - 1] = '\0';

            // Copy the line in the content variable
            strcat(content[i], line);
        }

        // printf("Contenu fichier = \n%s %d \n", content[i], i);

        // printf(" finis lecture \n");

        fclose(input);

        int recv = i % (size_r);


        if (recv == 0)
            recv++;

        MPI_Send(content[i], strlen(content[i]), MPI_CHAR, recv, 0, MPI_COMM_WORLD);
        i++;
    }
    i = 0;

    for (int j = 1; i < size_r; i++)
        MPI_Send(&i, 1, MPI_INT, j, 1, MPI_COMM_WORLD);
    // Free everything
    free(content);
    if (closedir(dir) == -1)
        return printf("Error close dir\n"), -1;
}

void getfile(int rank)
{
    MPI_Status sta;
    MPI_Request req;
    int flag;

    int cont = 1;

    while (cont)
    {
        MPI_Status status;
        int count;
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (sta.MPI_TAG == 1)
        {
            MPI_Recv(&count, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            return;
        }

        MPI_Get_count(&status, MPI_CHAR, &count);

        char *seq = (char *)malloc(sizeof(char) * count);

        MPI_Recv(seq, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

      //  printf("%d) fichier = %s\n", rank, seq);
    }
}

int main(int argc, char **argv)
{
    int RANK_MASTER = 0;

    MPI_Init(&argc, &argv);

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == RANK_MASTER)
    {
        readfiles(size);
    }
    else
    {
        getfile(rank);
    }

    MPI_Finalize();

    return 0;
}