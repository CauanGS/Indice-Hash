#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 0

/*
if(DEBUG)
{
    printf("\n");
}
*/

#define YES 1
#define NO 0
#define BOF_INVALID -1

typedef struct 
// Guarda o header do arquivo binario
{
    int regLidos;
    int regBuscados; // aqui ta estranho no dados.bin
    int regRemovidos;
}HEADER;

typedef struct 
// Guarda os registros do inserir.bin
{
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;
}REGISTRO;

typedef struct
{
    char id_aluno[4];
    char sigla_disc[4];
    int BOF;
}CHAVE;



//------EstruturaDoHash------//

#define BUCKET_SIZE 2
#define HASH_TABLE_SIZE 13 // Tamanho da tabela hash


typedef struct
{
    CHAVE chave[BUCKET_SIZE];
} BUCKET;

extern BUCKET* hashTable[HASH_TABLE_SIZE];

// Estrutura da tabela hash
// typedef struct HashNode
// {
//     BUCKET bucket; // Cada no pode conter 2 chaves (BUCKET)
//     struct HashNode* next;
// } HashNode;


//talvez isso precise ser tratado de forma diferente (ou seja possivel erro)

//------FuncoesHash.c------//

void imprimeMenu();
int compararChaves(CHAVE key1, CHAVE key2);
void carregaArquivos();
FILE *abrirArquivo(char *filename);
int inserir();
void criar_hash();
int hashing_chave(CHAVE *chave);
int inserir_chave(CHAVE *chave);
void carregarEmMemoria();
int buscarHash();
void removerHash();

// void carregaArquivos();
// void inserir();
// FILE *abrirArquivo(char *filename);
// void imprimeMenu();
// int compararChaves(CHAVE key1, CHAVE key2);