#include <stdio.h>
#include <stdlib.h>
#include "rdtsc.h"
#include "../versions/bin/headers/gene_bin_omp.h"
#include <string.h>
#include <omp.h>
#include <mpi.h>

#define MAX 31000
#define MAX_LINE 150
#define MAX_LOOP 1

void load_gene(char *filename, char *seq_char)
{
	char temp[MAX_LINE] = "";
	FILE *file;
	file = fopen(filename, "r");
	if(!file)
		printf("ERROR: load_gene: cannot open file %s\n", filename);

	if(file)
    {
		fgets(temp, MAX_LINE, file); 
        while(fgets(temp, MAX_LINE, file) != NULL)
        {
        	temp[strlen(temp)-1] = '\0';
        	strcat(seq_char, temp);
        }
    }
	//fscanf(file, "%s", seq_char);

	fclose(file);
}

int main(int argc, char *argv[])
{
	MPI_Init(NULL,NULL);
	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double before = 0;
	double after = 0;
	double elapsed = 0.0;

	char seq_char[MAX] = "";
	load_gene("LC528232.1.fasta", seq_char);
	// printf("%s", seq_char);
	unsigned long long seq_char_size = strlen(seq_char);

	char seq_char2[MAX] = "";
	load_gene("MN908947.3.fasta", seq_char2);
	// printf("%s", seq_char2);
	unsigned long long seq_char_size2 = strlen(seq_char2);


	// Variable used for function
	long int *seq_long = NULL;
	long int *seq_long2 = NULL;
	float cms = 0;
	char *rna_seq_long = NULL;
	char *aa_seq_long = NULL;
	char *seq_test = calloc(seq_char_size, sizeof(char));
	
	
  	
  	gene_map_t g;
    g.gene_start = malloc(sizeof(*g.gene_start) * seq_char_size * int_SIZE);
    g.gene_end = malloc(sizeof(*g.gene_end) * seq_char_size * int_SIZE);
    
    mutation_map m;
	m.size = malloc(sizeof(unsigned long) * 5);
	m.start_mut = malloc(sizeof(unsigned long) * 5);
	m.end_mut = malloc(sizeof(unsigned long) * 5);
	for(int i = 0; i < 5; i++)
	{
		m.size[i]=0;
    	m.start_mut[i]=0;
    	m.end_mut[i]=0;   	
    }

    long int* seq_bin;
        // Number of bits needed to transform seq_char into a binary array.
    int seq_bin_size = 2 * seq_char_size;

    // Binary array new size
    int nb = seq_bin_size / int_SIZE;
    if(seq_bin_size % int_SIZE != 0)
        nb++;

    // Allocate memory and verify it has been allocated
    seq_bin = calloc(nb, sizeof(long int));

    //Allocate memory and verify it has been allocated
    char* dna_seq = calloc(seq_char_size + 1, sizeof(char));

    // Allocate memory and verify it has been allocated
    char* rna_seq = malloc(sizeof(char) * seq_char_size + 2);

    // Allocate memory and verify it has been allocated
    char* aa_seq = calloc(sizeof(char), sizeof(char) * (seq_char_size / 3) + 1);

    seq_long2 = convert_to_binary(seq_char2, seq_char_size2);

    printf("Binaries Functions\t    | Cycles\n");
    printf("-----------------------------------------\n");

#pragma omp parallel default(shared) private(seq_long2, seq_long, seq_test, rna_seq_long, aa_seq_long, cms)
{
	// seq_long2 = convert_to_binary(seq_char2, seq_char_size2);

	/*-----convert_to_binary-----*/
	before = omp_get_wtime();
	for(int i = 0; i < MAX_LOOP; i++)
	{
    	seq_long = set_binary_array(seq_char, seq_char_size);
    	// set_binary_arr(seq_char, seq_char_size, seq_long);
	}
	after = omp_get_wtime();
#pragma omp single
{
    elapsed = (double)(after - before);
	printf("convert_to_binary\t\t : %.10lf\n", elapsed/MAX_LOOP );
}
	elapsed = 0;

	/*-----binary_to_dna-----*/
	before = omp_get_wtime();
	for(int i = 0; i < MAX_LOOP; i++)
	{
    	// seq_test = binary_to_dna(seq_long, 2 * seq_char_size);
    	bin_to_dna(seq_long, 2 * seq_char_size, dna_seq);
	}
    after = omp_get_wtime();
#pragma omp single
{
    elapsed = (double)(after - before);
	printf("binary_to_dna\t\t\t : %.10lf\n", elapsed/MAX_LOOP );
}
	elapsed = 0;

	/*-----generating_mRNA-----*/
	before = omp_get_wtime();
	for(int i = 0; i < MAX_LOOP; i++)
	{
		// rna_seq_long = generating_mRNA(seq_long, 0, 2 * seq_char_size);
		gen_mRNA(seq_long, 0, 2 * seq_char_size, rna_seq);

	}
    after = omp_get_wtime();
#pragma omp single
{
    elapsed = (double)(after - before);
	printf("generating_mRNA\t\t\t : %.10lf\n", elapsed/MAX_LOOP );
}
	elapsed = 0;
	
	/*-----detecting_genes-----*/
#pragma omp single
{
	before = omp_get_wtime();
	for(int i = 0; i < MAX_LOOP; i++)
	{
		detecting_genes(seq_long, 2 * seq_char_size, &g);
	}
	after = omp_get_wtime();
    elapsed = (double)(after - before);
	printf("detecting_genes\t\t\t : %.10lf\n", elapsed/MAX_LOOP );
}
	elapsed = 0;
	
	/*-----generating_amino_acid_chain-----*/
	before = omp_get_wtime();
	for(int i = 0; i < MAX_LOOP; i++)
	{
		// aa_seq_long = generating_amino_acid_chain(seq_long, 0, 2 * seq_char_size);
		gen_aa_chain(seq_long, 0, 2 * seq_char_size, aa_seq);
	}
    after = omp_get_wtime();
#pragma omp single
{
    elapsed = (double)(after - before);
	printf("generating_amino_acid_chain\t : %.10lf\n", elapsed/MAX_LOOP );
}
	elapsed = 0;

	/*-----detecting_mutations-----*/
#pragma omp single
{
	before = omp_get_wtime();
	for(int i = 0; i < MAX_LOOP; i++)
	{
    	detecting_mutations(seq_long, 0, 2 * seq_char_size, m);
	}
    after = omp_get_wtime();
    elapsed = (double)(after - before);
	printf("detecting_mutations\t\t : %.10lf\n", elapsed/MAX_LOOP );
}
	elapsed = 0;

	/*-----calculating_matching_score-----*/
#pragma omp single
{
	before = omp_get_wtime();
	for(int i = 0; i < MAX_LOOP; i++)
	{
		cms = calculating_matching_score(seq_long, 0, 2 * seq_char_size, seq_long2, 0, 2 * seq_char_size2);
	}
    after = omp_get_wtime();
    elapsed = (double)(after - before);
	printf("calculating_matching_score\t : %.10lf\n", elapsed/MAX_LOOP );
}
	printf("\n");

}
	// free
	free(g.gene_start);
	free(g.gene_end);
	free(m.size);
	free(m.start_mut);
	free(m.end_mut);
	MPI_Finalize();

	return 0;
}
