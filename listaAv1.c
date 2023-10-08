#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nome[20];
    char genero[20];
    char local[30];
    char **albuns;
    int num_albuns;
} Artista;

Artista lerArtista(FILE *arquivo) {
    Artista artista;
    fgets(artista.nome, sizeof(artista.nome), arquivo);
    fgets(artista.genero, sizeof(artista.genero), arquivo);
    fgets(artista.local, sizeof(artista.local), arquivo);
    artista.nome[strcspn(artista.nome, "\n")] = '\0';
    artista.genero[strcspn(artista.genero, "\n")] = '\0';
    artista.local[strcspn(artista.local, "\n")] = '\0';

    artista.num_albuns = 0;
    artista.albuns = NULL;
    char linha[300];
    while (fgets(linha, sizeof(linha), arquivo) && strcmp(linha, "===========\n") != 0) {
        linha[strcspn(linha, "\n")] = '\0';
        artista.num_albuns++;
        artista.albuns = (char **)realloc(artista.albuns, artista.num_albuns * sizeof(char *));
        artista.albuns[artista.num_albuns - 1] = strdup(linha);
    }
    return artista;
}

void escreverArtista(FILE *arquivo, Artista artista) {
    fprintf(arquivo, "%s\n", artista.nome);
    fprintf(arquivo, "%s\n", artista.genero);
    fprintf(arquivo, "%s\n", artista.local);
    for (int i = 0; i < artista.num_albuns; i++) {
        fprintf(arquivo, "%s\n", artista.albuns[i]);
    }
    fprintf(arquivo, "===========\n");
}

void inserirArtistaOrdenado(Artista **lista, int *num_artistas, Artista novo) {
    (*num_artistas)++;
    (*lista) = (Artista *)realloc((*lista), (*num_artistas) * sizeof(Artista));
    int i = (*num_artistas) - 2;

    while (i >= 0 && strcmp(novo.nome, (*lista)[i].nome) < 0) {
        (*lista)[i + 1] = (*lista)[i];
        i--;
    }
    (*lista)[i + 1] = novo;
}

int buscarArtistaBinario(Artista *lista, int num_artistas, char *nome) {
    int inicio = 0;
    int fim = num_artistas - 1;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        int comparacao = strcmp(lista[meio].nome, nome);

        if (comparacao == 0) {
            return meio;
        } else if (comparacao < 0) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    return -1;
}

int buscarArtistaSequencial(Artista *lista, int num_artistas, char *nome) {
    for (int i = 0; i < num_artistas; i++) {
        if (strcmp(lista[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

void removerArtista(Artista **lista, int *num_artistas, char *nome) {
    int indice = buscarArtistaBinario(*lista, *num_artistas, nome);
    if (indice == -1) {
        printf("Artista nao encontrado.\n");
        return;
    }

    free((*lista)[indice].albuns);
    for (int i = indice; i < (*num_artistas) - 1; i++) {
        (*lista)[i] = (*lista)[i + 1];
    }
    (*num_artistas)--;
    (*lista) = (Artista *)realloc((*lista), (*num_artistas) * sizeof(Artista));

    FILE *arquivo = fopen("artistas.txt", "w");
    for (int i = 0; i < (*num_artistas); i++) {
        escreverArtista(arquivo, (*lista)[i]);
    }
    fclose(arquivo);
}

void editarArtista(Artista *lista, int num_artistas, char *nome) {
    int indice = buscarArtistaBinario(lista, num_artistas, nome);
    if (indice == -1) {
        printf("Artista nao encontrado.\n");
        return;
    }

    printf("Digite as novas informacoes do artista:\n");
    printf("Novo nome: ");
    fgets(lista[indice].nome, sizeof(lista[indice].nome), stdin);
    printf("Novo genero: ");
    fgets(lista[indice].genero, sizeof(lista[indice].genero), stdin);
    printf("Novo local: ");
    fgets(lista[indice].local, sizeof(lista[indice].local), stdin);
    lista[indice].nome[strcspn(lista[indice].nome, "\n")] = '\0';
    lista[indice].genero[strcspn(lista[indice].genero, "\n")] = '\0';
    lista[indice].local[strcspn(lista[indice].local, "\n")] = '\0';

    for (int i = 0; i < lista[indice].num_albuns; i++) {
        free(lista[indice].albuns[i]);
    }
    lista[indice].num_albuns = 0;
    free(lista[indice].albuns);
    lista[indice].albuns = NULL;

    printf("Digite os albuns (digite 'fim' para encerrar):\n");
    char linha[300];
    while (1) {
        fgets(linha, sizeof(linha), stdin);
        linha[strcspn(linha, "\n")] = '\0';
        if (strcmp(linha, "fim") == 0) {
            break;
        }
        lista[indice].num_albuns++;
        lista[indice].albuns = (char **)realloc(lista[indice].albuns, lista[indice].num_albuns * sizeof(char *));
        lista[indice].albuns[lista[indice].num_albuns - 1] = strdup(linha);
    }

    FILE *arquivo = fopen("artistas.txt", "w");
    for (int i = 0; i < num_artistas; i++) {
        escreverArtista(arquivo, lista[i]);
    }
    fclose(arquivo);
}

int main() {
    FILE *arquivo = fopen("artistas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    Artista *artistas = NULL;
    int num_artistas = 0;
    char linha[300];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strcmp(linha, "===========\n") == 0) {
            Artista artista = lerArtista(arquivo);
            inserirArtistaOrdenado(&artistas, &num_artistas, artista);
        }
    }
    fclose(arquivo);

    int escolha;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Insercao ordenada de um artista\n");
        printf("2. Remocao de um artista\n");
        printf("3. Edicao de um artista\n");
        printf("4. Busca binaria por um artista\n");
        printf("5. Busca sequencial por um album\n");
        printf("6. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &escolha);
        getchar();

        switch (escolha) {
            case 1: {
                Artista novo;
                printf("Digite as informacoes do novo artista:\n");
                printf("Nome: ");
                fgets(novo.nome, sizeof(novo.nome), stdin);
                printf("Genero: ");
                fgets(novo.genero, sizeof(novo.genero), stdin);
                printf("Local: ");
                fgets(novo.local, sizeof(novo.local), stdin);
                novo.nome[strcspn(novo.nome, "\n")] = '\0';
                novo.genero[strcspn(novo.genero, "\n")] = '\0';
                novo.local[strcspn(novo.local, "\n")] = '\0';

                printf("Digite os albuns (digite 'fim' para encerrar):\n");
                char linha[100];
                while (1) {
                    fgets(linha, sizeof(linha), stdin);
                    linha[strcspn(linha, "\n")] = '\0';
                    if (strcmp(linha, "fim") == 0) {
                        break;
                    }
                    novo.num_albuns++;
                    novo.albuns = (char **)realloc(novo.albuns, novo.num_albuns * sizeof(char *));
                    novo.albuns[novo.num_albuns - 1] = strdup(linha);
                }

                inserirArtistaOrdenado(&artistas, &num_artistas, novo);

                FILE *arquivo = fopen("artistas.txt", "a");
                escreverArtista(arquivo, novo);
                fclose(arquivo);
                break;
            }
            case 2: {
                char nome[100];
                printf("Digite o nome do artista a ser removido: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                removerArtista(&artistas, &num_artistas, nome);
                break;
            }
            case 3: {
                char nome[100];
                printf("Digite o nome do artista a ser editado: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                editarArtista(artistas, num_artistas, nome);
                break;
            }
            case 4: {
                char nome[100];
                printf("Digite o nome do artista a ser buscado: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                int indice = buscarArtistaBinario(artistas, num_artistas, nome);
                if (indice != -1) {
                    printf("Artista encontrado:\n");
                    printf("Nome: %s\n", artistas[indice].nome);
                    printf("Genero: %s\n", artistas[indice].genero);
                    printf("Local: %s\n", artistas[indice].local);
                    printf("Albuns:\n");
                    for (int i = 0; i < artistas[indice].num_albuns; i++) {
                        printf("%s\n", artistas[indice].albuns[i]);
                    }
                } else {
                    printf("Artista nao encontrado.\n");
                }
                break;
            }
            case 5: {
                char album[100];
                printf("Digite o nome do album a ser buscado: ");
                fgets(album, sizeof(album), stdin);
                album[strcspn(album, "\n")] = '\0';

                int encontrado = 0;
                for (int i = 0; i < num_artistas; i++) {
                    for (int j = 0; j < artistas[i].num_albuns; j++) {
                        if (strcmp(artistas[i].albuns[j], album) == 0) {
                            printf("Album encontrado no artista %s:\n", artistas[i].nome);
                            printf("Nome: %s\n", artistas[i].nome);
                            printf("Genero: %s\n", artistas[i].genero);
                            printf("Local: %s\n", artistas[i].local);
                            printf("Albuns:\n");
                            for (int k = 0; k < artistas[i].num_albuns; k++) {
                                printf("%s\n", artistas[i].albuns[k]);
                            }
                            encontrado = 1;
                            break;
                        }
                    }
                    if (encontrado) {
                        break;
                    }
                }

                if (!encontrado) {
                    printf("Album nao encontrado em nenhum artista.\n");
                }
                break;
            }
            case 6:
                for (int i = 0; i < num_artistas; i++) {
                    free(artistas[i].albuns);
                }
                free(artistas);
                return 0;
            default:
                printf("Opcao invalida.\n");
                break;
        }
    }

    return 0;
}
