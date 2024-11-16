#include "hash.h"

int main()
{
    short operacaoDesejada;
    short numInsert;
    int inseridosComSucesso;

    HEADER header;
    FILE *dados = NULL;
    FILE *hash = NULL;

    if ((dados = fopen("dados.bin", "r+b")) == NULL)
    {
        if ((dados = fopen("dados.bin", "w+b")) != NULL)
        // Caso arquivos de dados nao esxista cria um novo arquivo de dados
        {
            printf("Nenhum arquivo de dados encontrado, criando novo arquivo de dados.\n");
            header.regBuscados = 0;
            header.regLidos = 0;
            header.regRemovidos = 0;

            fwrite(&header, sizeof(header), 1, dados);
            if (DEBUG)
            {
                printf("tamanho do header: %ld\n", (long)sizeof(header));
                printf("registros lidos: %d\n", header.regLidos);
                printf("registros buscados: %d\n", header.regBuscados);
                printf("registros removidos: %d\n", header.regRemovidos);
            }

            fclose(dados);
            
            printf("Novo arquivo de dados criado com sucesso!\n");
        }
        else
        {
            printf("Nao foi possivel criar o arquivo dados.\n");
            return 0;
        }
    }else{
        printf("Arquivo de dados existente encontrado.\n");
    }

    if ((hash = fopen("hash.bin", "r+b")) == NULL)
    {
        printf("Arquivo hash nao encontrado, criando novo arquivo hash...\n");
        criar_hash();
    }else{
        printf("Arquivo de hash existente encontrado.\n");
    }

    carregarEmMemoria();

    while (YES)
    {

        // Menu principal
        imprimeMenu();
        printf("Selecione a opcao desejada: ");
        scanf("%hd", &operacaoDesejada);
        switch (operacaoDesejada)
        {
        case 1:
            // Inserir registros no indice e no arquivo de dados
            printf("Insira a quantidade de insercoes desejada: ");
            scanf("%hd", &numInsert);
            inseridosComSucesso = 0;

            for (int i = 0; i < numInsert; i++)
            {
                printf("\n-------------------\n");
                inseridosComSucesso = inserir() + inseridosComSucesso;
                printf("-------------------\n\n");
            }

            printf("\n%d registro(s) inserido(s) com sucesso!\n", inseridosComSucesso); // num pode ser o numero de insert, e sim o numero de de insercoes realizadas com sucesso
            break;
        case 2:
            // Buscar por uma chave no indice e no arquivo de dados
            buscarHash();
            break;
        case 3:
            // Remover uma chave do indice
            printf("Insira qual indice voce deseja remover: \n");
            removerHash();
            break;
        case 4:
            // Salvar e Encerrar o programa
            printf("\nSalvando e Encerrando o programa...");
            return 0;
            break;
        case 5:
            // Carregar Arquivos de Input, Busca e Remover
            carregaArquivos();
            break;
        case 6:
            printf("Debugando: \n");

            break;
        default:
            printf("\nFormato invalido, verifique o numero correspondente a opcao desejada e tente novamente:\n");
            break;
        }
    }

    return 0;
}