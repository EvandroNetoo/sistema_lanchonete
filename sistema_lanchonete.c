#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdbool.h>

#define STRING_LEN 51

typedef struct {
    char nome[STRING_LEN];
    float preco;
    char ingredientes[][STRING_LEN];
} Lanche;

typedef struct {
    int numero;
    char cliente[STRING_LEN];
    int quantidade;
    Lanche lanche;
} Pedido;

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void menu(int *opcao) {
    // system("cls");
    printf("\n\n");
    printf("SISTEMA DE LANCHONETE\n");
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

void input_numero_positivo(char tipo, void *numero, char *label, char *msg_error) {
    bool tentou = false;
    do {
        if (tentou) {
            printf("%s\n", msg_error);
        }
        printf("%s", label);

        if (tipo == 'i') {  // Se for int
            scanf("%d", (int *)numero);
        } else if (tipo == 'f') {  // Se for float
            scanf("%f", (float *)numero);
        }

        tentou = true;

    } while ((tipo == 'i' && *(int *)numero <= 0) || (tipo == 'f' && *(float *)numero <= 0.0));
}




Lanche input_lanche() {
    Lanche lanche;
    strcpy(lanche.nome, input_string("Nome: ")); 
    input_numero_positivo('f', &lanche.preco, "Preço: ", "O preço deve ser maior que 0.");
    return lanche;
}

Pedido input_pedido() {
    Pedido pedido;

    input_numero_positivo('i', &pedido.numero, "Número do pedido: ", "O número do pedido deve ser maior que 0.");
    strcpy(pedido.cliente, input_string("Nome do cliente: "));
    input_numero_positivo('i', &pedido.numero, "Número do pedido: ", "O número do pedido deve ser maior que 0.");
    return pedido;
}

int main() {
    SetConsoleOutputCP(65001);

    int opcao;

    do {
        menu(&opcao);

        switch (opcao)
        {
        case 1:
            input_pedido();
            break;
        
        case 0:
            break;
        default:
            printf("Opção inválida.\n");
            break;
        }
        
        if (opcao != 0) {
            printf("Pressione <ENTER> para continuar...");
            getchar();
        }
    } while(opcao != 0);


    return 0;
}