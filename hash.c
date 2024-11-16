#include "hash.h"

BUCKET *hashTable[HASH_TABLE_SIZE];

void imprimeMenu()
{
    printf("\n===============MENU===============\n\n");
    printf("1 - Inserir registros no indice e no arquivo de dados\n");
    printf("2 - Buscar por uma chave no indice e no arquivo de dados\n");
    printf("3 - Remover uma chave do indice\n");
    printf("4 - Salvar e Encerrar o programa\n");
    printf("5 - Carregar Arquivos de Input, Busca e Remove\n");
    printf("\n==================================\n");
}

int compararChaves(CHAVE key1, CHAVE key2)
{
    if (DEBUG)
    {
        printf("Comparando chaves...\n%s#%s \n%s#%s\n", key1.id_aluno, key1.sigla_disc, key2.id_aluno, key2.sigla_disc);
    }

    char id1[9], id2[9];
    sprintf(id1, "%s#%s", key1.id_aluno, key1.sigla_disc);
    sprintf(id2, "%s#%s", key2.id_aluno, key2.sigla_disc);

    if (DEBUG)
    {
        printf("Retornando resultado de strcmp: %d\n", strcmp(id1, id2));
    }
    return strcmp(id1, id2);
}

void carregaArquivos()
{
    FILE *fd;

    //////////////////////////////
    struct hist
    {
        char id_aluno[4];
        char sigla_disc[4];
        char nome_aluno[50];
        char nome_disc[50];
        float media;
        float freq;
    } vet[7] = {{"000", "001", "Nome-1", "Disc-111", 1, 1},
                {"000", "014", "Nome-14", "Disc-222", 2, 2},
                {"000", "027", "Nome-27", "Disc-333", 3, 3},
                {"000", "040", "Nome-40", "Disc-444", 4, 4},
                {"000", "053", "Nome-53", "Disc-555", 5, 5},
                {"000", "066", "Nome-66", "Disc-666", 6, 6},
                {"000", "053", "Nome-53", "Disc-777", 7, 7}}; // chave duplicada

    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);

    //////////////////////////////
    struct busca
    {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_b[5] = {{"000", "014"},
                  {"000", "040"},
                  {"000", "053"},
                  {"000", "066"},
                  {"000", "066"}};

    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);

    //////////////////////////////
    struct remove
    {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_r[2] = {{"000", "014"},
                  {"000", "040"}};

    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

FILE *abrirArquivo(char *filename)
// Abrir arquivos binarios
{
    FILE *file;
    if ((file = fopen(filename, "r+b")) == NULL)
    {
        if ((file = fopen(filename, "w+b")) == NULL)
        {
            printf("O arquivo nao pode ser aberto [%s]", filename);
            exit(1);
        }
    }
    return file;
}

void carregarEmMemoria()
{
    FILE *hash = abrirArquivo("hash.bin");
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        fread(&hashTable[i], sizeof(BUCKET), 1, hash);
    }
    fclose(hash);
}

int inserir()
{
    FILE *insere;
    FILE *dados;

    REGISTRO registro;
    HEADER header;
    CHAVE chave;

    CHAVE nova_chave;

    int pos_reg = 0;
    int tam_reg = 0;

    char buffer[500];

    insere = abrirArquivo("insere.bin");
    dados = abrirArquivo("dados.bin");

    // Le o header do arquivo de dados
    fread(&header, sizeof(HEADER), 1, dados);

    // Move ate o proximo registro a ser inserido
    fseek(insere, (header.regLidos) * sizeof(registro), SEEK_SET);

    if (fgetc(insere) == EOF)
    // Testa se o valor do indice nao apontou para um indice inexistente
    {
        printf("\nEsse indice nao esta disponivel\n");
        return NO;
    }

    fseek(insere, -1, SEEK_CUR);

    fread(&registro, sizeof(registro), 1, insere);
    snprintf(buffer, sizeof(buffer), "%s#%s#%s#%s#%.2f#%.2f", registro.id_aluno, registro.sigla_disc, registro.nome_aluno, registro.nome_disc, registro.media, registro.freq);

    strcpy(nova_chave.id_aluno, registro.id_aluno);
    strcpy(nova_chave.sigla_disc, registro.sigla_disc);

    fseek(insere, (header.regLidos) * sizeof(chave), SEEK_SET);

    fseek(dados, 0, SEEK_END);

    pos_reg = (int)ftell(dados);
    tam_reg = strlen(buffer);
    header.regLidos++;
    nova_chave.BOF = pos_reg;

    if (!inserir_chave(&nova_chave))
    {
        printf("Nao foi possivel realizar a insercao\n");
        return NO;
    }

    // Escreve o registro no arquivo de dados
    fwrite(&tam_reg, sizeof(int), 1, dados);
    fwrite(buffer, tam_reg, 1, dados);

    rewind(dados);
    fwrite(&header, sizeof(HEADER), 1, dados);

    printf("\nRegistro incluido com sucesso no arquivo de dados!\n");

    fclose(insere);
    fclose(dados);
    return YES;
}

//----------FuncoesDoHash----------//

void criar_hash()
{
    FILE *hash;
    if ((hash = fopen("hash.bin", "r+b")) == NULL)
    // Teste se ja nao existe um arquivo de hash
    {
        hash = fopen("hash.bin", "w+b");
        BUCKET bucket;
        for (int i = 0; i < BUCKET_SIZE; i++)
        // Preenche o bucket com as informacoes nulas
        {
            strcpy(bucket.chave[i].id_aluno, "///");
            strcpy(bucket.chave[i].sigla_disc, "///");
            bucket.chave[i].BOF = BOF_INVALID;
        }
        rewind(hash);
        for (int i = 0; i < HASH_TABLE_SIZE; i++)
        {
            fwrite(&bucket, sizeof(BUCKET), 1, hash);
        }
        printf("Arquivo hash.bin criado com sucesso\n");
        fclose(hash);
    }
    else
    {
        printf("O arquivo hash.bin ja existe\n");
    }
}

int hashing_chave(CHAVE *chave)
// Funcao de hashing que utiliza add e fold
{

    char chave_concat[9];
    strcpy(chave_concat, chave->id_aluno);
    strcat(chave_concat, chave->sigla_disc);

    int endereco = 0;
    int soma = 0;
    int par = 0;
    int i = 0;

    while (chave_concat[i] != '\0' && chave_concat[i + 1] != '\0')
    {
        par = (chave_concat[i] * 100) + chave_concat[i + 1];
        if (DEBUG)
        {
            printf("par: %d\n", par);
        }
        soma += par;
        i += 2;
    }
    if (chave_concat[i] != '\0')
    {
        soma += chave_concat[i];
    }
    endereco = soma % HASH_TABLE_SIZE;

    return endereco;
}

int inserir_chave(CHAVE *chave)
{
    int end_reg_eliminado = -1; // Marca a posicao de um registro eliminado para ser preenchido caso necessario
    BUCKET bucket;
    int count = 0;
    int inseriu = 0;
    BUCKET bucketAux;
    CHAVE chaveAux = *chave;
    FILE *dados;
    HEADER header;
    int endereco_hash = hashing_chave(chave);

    FILE *hash = abrirArquivo("hash.bin");
    dados = abrirArquivo("dados.bin");

    CHAVE chaveEliminada;

    strcpy(chaveEliminada.id_aluno, "###");
    strcpy(chaveEliminada.sigla_disc, "###");
    chaveEliminada.BOF = BOF_INVALID;

    // Le o header do arquivo de dados
    fread(&header, sizeof(HEADER), 1, dados);

    printf("Chave: %s#%s\n", chave->id_aluno, chave->sigla_disc);

    while (YES)
    {
        fseek(hash, (endereco_hash * sizeof(bucketAux)), SEEK_SET);

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            fread(&bucketAux.chave[i], sizeof(CHAVE), 1, hash);
        }

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            if (compararChaves(bucketAux.chave[i], chaveEliminada) == 0)
            {
                if (DEBUG == 1)
                {
                    printf("Chave eliminada encontrada\n");
                }
                end_reg_eliminado = ftell(hash) - (BUCKET_SIZE - (i * sizeof(CHAVE)));
                if (DEBUG == 1)
                {
                    printf("Endereco do registro eliminado: %d\n", end_reg_eliminado);
                }
                break;
            }
            if (compararChaves(bucketAux.chave[i], chaveAux) == 0)
            {
                printf("Falha na insercao... Chave ja existente\n");

                printf("Chave: %s#%s encontrada no endereco %d, %d acessos\n", bucketAux.chave[i].id_aluno, bucketAux.chave[i].sigla_disc, endereco_hash, i + 1);

                header.regLidos++;
                rewind(dados);
                fwrite(&header, sizeof(header), 1, dados);
                fclose(dados);
                return NO;
            }
        }

        endereco_hash = (endereco_hash + 1) % HASH_TABLE_SIZE;

        if (endereco_hash == hashing_chave(&chaveAux))
        {
            if (DEBUG)
            {
                printf("Chave nao encontrada, prosseguindo com a insercao...\n");
            }
            break;
        }
    }

    while (YES)
    {
        printf("Endereco hash: %d\n", endereco_hash);
        fseek(hash, (endereco_hash * sizeof(BUCKET)), SEEK_SET);
        fread(&bucket, sizeof(BUCKET), 1, hash);

        inseriu = 0;

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            if (bucket.chave[i].BOF == -1)
            {
                bucket.chave[i] = *chave;
                fseek(hash, (endereco_hash * sizeof(BUCKET)), SEEK_SET);
                fwrite(&bucket, sizeof(BUCKET), 1, hash);
                printf("Chave: %s#%s inserida com sucesso!\n", bucket.chave[i].id_aluno, bucket.chave[i].sigla_disc);
                inseriu = 1;
                break;
            }
        }

        if (inseriu == 1)
        {
            break;
        }

        printf("Houve uma colisao no endereco %d.\n", endereco_hash);

        endereco_hash = (endereco_hash + 1) % HASH_TABLE_SIZE; // Tratamento de colisao circular

        if (end_reg_eliminado != -1)
        {
            fseek(hash, end_reg_eliminado, SEEK_SET);
            fwrite(&chaveAux, sizeof(CHAVE), 1, hash);
            printf("Chave: %s#%s inserida com sucesso em um espaço vago!\n", chaveAux.id_aluno, chaveAux.sigla_disc);
            break;
        }

        if (endereco_hash == hashing_chave(chave))
        {
            printf("O indice hash esta cheio.\n");
            return NO;
        }

        printf("Tentativa: %d\n\n", ++count);
    }

    fclose(hash);
    return YES;
}

int buscarHash()
{
    BUCKET bucket;
    int endereco_hash = 0;
    int count = 0;
    int tam_reg = 0;
    FILE *dados = abrirArquivo("dados.bin");
    FILE *busca = abrirArquivo("busca.bin");
    FILE *hash = abrirArquivo("hash.bin");
    HEADER header;
    CHAVE chave;
    CHAVE chaveIntocada;
    char buffer[256];

    strcpy(chaveIntocada.id_aluno, "///");
    strcpy(chaveIntocada.sigla_disc, "///");
    chaveIntocada.BOF = BOF_INVALID;

    fread(&header, sizeof(header), 1, dados);

    fseek(busca, (header.regBuscados) * (sizeof(chave) - sizeof(int)), SEEK_SET);

    if (DEBUG)
    {
        printf("Header.regBuscados: %d\n", header.regBuscados);
    }

    if (fgetc(busca) == EOF)
    {
        printf("\nEsse indice nao esta disponivel\n");
        return NO;
    }
    fseek(busca, -1, SEEK_CUR);

    fread(&chave, sizeof(chave) - sizeof(int), 1, busca);

    printf("Chave: %s#%s\n", chave.id_aluno, chave.sigla_disc);

    endereco_hash = hashing_chave(&chave);

    fseek(dados, 0, SEEK_SET);
    header.regBuscados++;
    fwrite(&header, sizeof(header), 1, dados);

    while (YES)
    {
        fseek(hash, (endereco_hash * sizeof(bucket)), SEEK_SET);

        if (DEBUG)
        {
            printf("posicao atual do hash: %ld\n", ftell(hash));
        }

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            fread(&bucket.chave[i], sizeof(CHAVE), 1, hash);
        }

        if (DEBUG)
        {
            for (int i = 0; i < BUCKET_SIZE; i++)
            {
                printf("buket.chave[%d].id_aluno: %s\n", i, bucket.chave[i].id_aluno);
                printf("buket.chave[%d].sigla_disc: %s\n", i, bucket.chave[i].sigla_disc);
            }
        }

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            if (compararChaves(bucket.chave[i], chave) == 0)
            {

                fseek(dados, bucket.chave[i].BOF, SEEK_SET);
                printf("ftell( dados 1 ): %ld\n", ftell(dados));
                fread(&tam_reg, sizeof(int), 1, dados);
                printf("ftell( dados 2 ): %ld\n", ftell(dados));
                fread(&buffer, tam_reg, 1, dados);
                printf("ftell( dados 3 ): %ld\n", ftell(dados));
                buffer[tam_reg] = '\0';

                printf("Chave: %s#%s encontrada no endereco %d, %d acessos\n", bucket.chave[i].id_aluno, bucket.chave[i].sigla_disc, endereco_hash, i + 1);

                printf("Registro encontrado: %s\n", buffer);

                fclose(dados);

                if (DEBUG)
                {
                    printf("Chave encontrada no endereco: %d; posicao: %d.\n", endereco_hash, i);
                }

                return YES;
            }
            else if (compararChaves(chaveIntocada, bucket.chave[i]) == 0)
            {
                printf("Chave nao encontrada.\n");
                fclose(dados);
                return NO;
            }
        }

        endereco_hash = (endereco_hash + 1) % HASH_TABLE_SIZE;

        if (endereco_hash == hashing_chave(&chave))
        {
            printf("Chave nao encontrada.\n");
            fclose(dados);
            return NO;
        }

        printf("Tentativa: %d\n\n", ++count);
    }

    fclose(hash);
}

void removerHash()
{
    BUCKET bucket;
    FILE *hash = abrirArquivo("hash.bin");

    int endereco_hash = 0;
    int count = 0;
    FILE *dados = abrirArquivo("dados.bin");
    FILE *remove = abrirArquivo("remove.bin");
    HEADER header;
    CHAVE chave;
    CHAVE nova_chave;
    int pos_inicio_registro = 0;

    fread(&header, sizeof(header), 1, dados);

    fseek(remove, (header.regRemovidos) * (sizeof(CHAVE) - sizeof(int)), SEEK_SET);

    printf("Header.regRemovidos: %d\n", header.regRemovidos);

    if (fgetc(remove) == EOF)
    {
        printf("\nEsse indice nao esta disponivel\n");
        return;
    }

    fseek(remove, -1, SEEK_CUR);
    ;
    fread(&chave, sizeof(chave) - sizeof(int), 1, remove);

    endereco_hash = hashing_chave(&chave);

    while (YES)
    {
        fseek(hash, (endereco_hash * sizeof(bucket)), SEEK_SET);

        pos_inicio_registro = ftell(hash);
        if (DEBUG)
        {
            printf("pos_inicio_registro: %d\n", pos_inicio_registro);
        }

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            fread(&bucket.chave[i], sizeof(bucket.chave[i]), 1, hash);
        }

        if (DEBUG)
        {
            for (int i = 0; i < BUCKET_SIZE; i++)
            {
                printf("buket.chave[%d].id_aluno: %s\n", i, bucket.chave[i].id_aluno);
                printf("buket.chave[%d].sigla_disc: %s\n", i, bucket.chave[i].sigla_disc);
            }
        }

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            if (compararChaves(bucket.chave[i], chave) == 0)
            {
                fseek(hash, pos_inicio_registro + (i * sizeof(chave)), SEEK_SET);
                strcpy(nova_chave.id_aluno, "###");
                strcpy(nova_chave.sigla_disc, "###");
                nova_chave.BOF = BOF_INVALID;

                fwrite(&nova_chave, sizeof(nova_chave), 1, hash);

                fseek(dados, 0, SEEK_SET);
                header.regRemovidos++;
                fwrite(&header, sizeof(header), 1, dados);

                fclose(dados);
                fclose(hash);
                printf("Chave removida no endereco: %d; posicao: %d.\n", endereco_hash, i);

                return;
            }
        }

        endereco_hash = (endereco_hash + 1) % HASH_TABLE_SIZE;

        if (endereco_hash == hashing_chave(&chave))
        {
            printf("Chave nao encontrada.\n");
            fseek(dados, 0, SEEK_SET);
            header.regRemovidos++;
            fwrite(&header, sizeof(header), 1, dados);
            fclose(hash);
            fclose(dados);
            return;
        }

        printf("Tentativa: %d\n\n", ++count);
    }

    fclose(hash);

}