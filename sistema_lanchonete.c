#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define STRING_LEN 51
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define CIANO "\x1b[36m"
#define GREEN "\x1b[32m"

typedef struct {
    char nome[STRING_LEN];
    float preco;
    int qtd_ingredientes;
    char **ingredientes;
} Lanche;

typedef struct {
    int numero;
    char cliente[STRING_LEN];
    int quantidade;
    Lanche lanche;
} Pedido;

typedef struct {
    int qtd_pedidos;
    Pedido *pedidos;
} Pedidos;

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_terminal() {
    system(_WIN32? "cls" : "clear");
}

void menu(int *opcao) {
    limpar_terminal();
    printf(CIANO "SISTEMA DE LANCHONETE\n" RESET);
    printf("1 - Inserir pedido\n");
    printf("2 - Pesquisar por número do pedido\n");
    printf("3 - Pesquisar por nome\n");
    printf("4 - Atualizar quantidade\n");
    printf("5 - Maior valor\n");
    printf("6 - Excluir pedido\n");
    printf("7 - Listar\n");
    printf("0 - Sair\n");
    printf("Escolha sua opção: ");
    scanf("%d", opcao);
}

char *input_string(char *label) {
    char *string = malloc(STRING_LEN);

    printf("%s", label);
    limpar_buffer();
    scanf("%50[^\n]", string);

    return string;
}

void input_numero_positivo(char tipo, void *numero, char *label,
                           char *msg_error) {
    bool tentou = false;
    do {
        if (tentou) {
            printf(RED "%s\n" RESET, msg_error);
        }
        printf("%s", label);

        if (tipo == 'i') { // Se for int
            scanf("%d", (int *)numero);
        } else if (tipo == 'f') { // Se for float
            scanf("%f", (float *)numero);
        }

        tentou = true;

    } while ((tipo == 'i' && *(int *)numero <= 0) ||
             (tipo == 'f' && *(float *)numero <= 0.0));
}

void input_ingredientes(Lanche *lanche) {
    printf("Quantidade de ingredientes: ");
    scanf("%d", &lanche->qtd_ingredientes);

    lanche->ingredientes = malloc(lanche->qtd_ingredientes * sizeof(char *));

    for (int i = 0; i < lanche->qtd_ingredientes; i++) {
        lanche->ingredientes[i] = malloc(STRING_LEN * sizeof(char));
        strcpy(lanche->ingredientes[i], input_string("Ingrediente: "));
    }
}

Lanche input_lanche() {
    Lanche lanche;
    strcpy(lanche.nome, input_string("Nome: "));
    input_numero_positivo('f', &lanche.preco,
                          "Preço: ", "O preço deve ser maior que 0.");
    input_ingredientes(&lanche);
    return lanche;
}

int buscar_pedido(Pedidos pedidos, void *busca, char campo) {
    switch (campo) {
    case 'n':
        for (int i = 0; i < pedidos.qtd_pedidos; i++) {
            if (pedidos.pedidos[i].numero == *(int *)busca) {
                return i;
            }
        }

        return -1;

    case 'c':
        for (int i = 0; i < pedidos.qtd_pedidos; i++) {
            if (strcmp(pedidos.pedidos[i].cliente, (char *)busca) == 0) {
                return i;
            }
        }
        return -1;

    default:
        return -1;
    }
}
Pedido input_pedido(Pedidos pedidos) {
    Pedido pedido;

    input_numero_positivo('i', &pedido.numero, "Número do pedido: ",
                          "O número do pedido deve ser maior que 0.");

    while (buscar_pedido(pedidos, &pedido.numero, 'n') != -1) {
        printf(RED "Esse número %d de pedido já foi inserido, Tente "
                   "novamente.\n" RESET,
               pedido.numero);
        input_numero_positivo('i', &pedido.numero, "Número do pedido: ",
                              "O número do pedido deve ser maior que 0.");
    }

    strcpy(pedido.cliente, input_string("Nome do cliente: "));
    input_numero_positivo('i', &pedido.quantidade, "Quantidade: ",
                          "A quantidade de lanches deve ser maior que 0.");

    printf("\nLanche: \n");
    pedido.lanche = input_lanche();

    return pedido;
}

void inserir_pedido(Pedidos *pedidos) {
    pedidos->pedidos = (Pedido *)realloc(
        pedidos->pedidos, (pedidos->qtd_pedidos + 1) * sizeof(Pedido));

    Pedido pedido = input_pedido(*pedidos);

    pedidos->pedidos[pedidos->qtd_pedidos] = pedido;

    pedidos->qtd_pedidos++;
}

void mostrar_pedidos(Pedidos pedidos) {
    if (pedidos.qtd_pedidos == 0) {
        printf("Não há pedidos cadastrados.\n");
        return;
    } else {
        printf("Número |  Cliente | Valor\n");
    }

    for (int i = 0; i < pedidos.qtd_pedidos; i++) {
        Pedido pedido = pedidos.pedidos[i];
        float valor_total = pedido.quantidade * pedido.lanche.preco;

        printf("%d | %s | R$ %.2f\n", pedido.numero, pedido.cliente,
               valor_total);
    }
}

void mostrar_pedido(Pedido pedido) {
    printf("Numero: %d\n", pedido.numero);
    printf("Cliente: %s\n", pedido.cliente);
    printf("Quantidade: %d\n", pedido.quantidade);
    printf("Lanche: %s\n", pedido.lanche.nome);
    printf("Preço: %.2f\n", pedido.lanche.preco);
    printf("Ingredientes: \n");
    for (int i = 0; i < pedido.lanche.qtd_ingredientes; i++) {
        printf("\t%s\n", pedido.lanche.ingredientes[i]);
    }
}

void excluir_pedido(Pedidos *pedidos, int indice) {
    int i;
    for (i = indice; i < pedidos->qtd_pedidos - 1; i++)
        pedidos->pedidos[i] = pedidos->pedidos[i + 1];
    pedidos->qtd_pedidos--;
    pedidos->pedidos = (Pedido *)realloc(pedidos->pedidos,
                                         pedidos->qtd_pedidos * sizeof(Pedido));
}

int i_maior_valor(Pedidos pedidos) {
    int i, i_maior_valor;
    float maior_valor = 0, valor_final;
    for (i = 0; i < pedidos.qtd_pedidos; i++) {
        valor_final =
            pedidos.pedidos[i].lanche.preco * pedidos.pedidos[i].quantidade;
        if (valor_final > maior_valor) {
            maior_valor = valor_final;
            i_maior_valor = i;
        }
    }
    return i_maior_valor;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    int opcao, busca, indice;
    char cliente[STRING_LEN];

    Pedidos pedidos;
    pedidos.qtd_pedidos = 0;
    pedidos.pedidos = malloc(0);

    do {
        menu(&opcao);

        switch (opcao) {
        case 1:
            limpar_terminal();
            printf(CIANO "INSERIR PEDIDO\n\n" RESET);
            inserir_pedido(&pedidos);
            printf(GREEN "\nPedido inserido com sucesso.\n" RESET);
            break;

        case 2:
            limpar_terminal();
            printf(CIANO "PESQUISAR POR NUMERO DO PEDIDO\n\n" RESET);
            input_numero_positivo('i', &busca, "Numero do pedido: ",
                                  "O número do pedido deve ser maior que 0.");
            indice = buscar_pedido(pedidos, &busca, 'n');
            if (indice == -1) {
                printf(RED "Nenhum pedido foi encontrado.\n" RESET);
            } else {
                mostrar_pedido(pedidos.pedidos[indice]);
            }
            break;

        case 3:
            limpar_terminal();
            printf(CIANO "PESQUISAR POR NOME\n\n" RESET);
            strcpy(cliente, input_string("Nome: "));
            indice = buscar_pedido(pedidos, cliente, 'c');
            if (indice == -1) {
                printf(RED "Nenhum pedido foi encontrado.\n" RESET);
            } else {
                mostrar_pedido(pedidos.pedidos[indice]);
            }
            break;

        case 4:
            limpar_terminal();
            printf(CIANO "ATUALIZAR QUANTIDADE\n\n" RESET);
            input_numero_positivo('i', &busca, "Numero do pedido: ",
                                  "O número do pedido deve ser maior que 0.");
            indice = buscar_pedido(pedidos, &busca, 'n');
            if (indice == -1) {
                printf(RED "Nenhum pedido foi encontrado.\n" RESET);
            } else {
                printf(GREEN "Quantidade atual: %d\n" RESET,
                       pedidos.pedidos[indice].quantidade);
                input_numero_positivo('i', &pedidos.pedidos[indice].quantidade,
                                      "Quantidade: ",
                                      "A quantidade de lanches deve ser maior "
                                      "que 0.");
                printf(GREEN "Quantidade atualizada com sucesso.\n" RESET);
            }
            break;

        case 5:
            limpar_terminal();
            printf(CIANO "MAIOR VALOR\n\n" RESET);
            indice = i_maior_valor(pedidos);
            mostrar_pedido(pedidos.pedidos[indice]);
            break;

        case 6:
            limpar_terminal();
            printf(CIANO "EXCLUIR PEDIDO\n\n" RESET);
            input_numero_positivo('i', &busca, "Numero do pedido: ",
                                  "O número do pedido deve ser maior que 0.");
            indice = buscar_pedido(pedidos, &busca, 'n');
            if (indice == -1) {
                printf(RED "Nenhum pedido foi encontrado.\n" RESET);
            } else {
                excluir_pedido(&pedidos, indice);
                printf(GREEN "Pedido excluído com sucesso.\n" RESET);
            }
            break;

        case 7:
            limpar_terminal();
            printf(CIANO "LISTA DE PEDIDOS\n\n" RESET);
            mostrar_pedidos(pedidos);
            break;

        case 0:
            break;

        default:
            printf(RED "Opção inválida.\n" RESET);
            break;
        }

        if (opcao != 0) {
            printf("\nPressione <ENTER> para continuar...");
            limpar_buffer();
            getchar();
        }
    } while (opcao != 0);

    return 0;
}