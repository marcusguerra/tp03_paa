#include <windows.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void SetColor(int ForgC){
     WORD wColor;
     //This handle is needed to get the current background attribute
     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO csbi;
     //csbi is used for wAttributes word

     if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
          //To mask out all but the background attribute, and to add the color
          wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
          SetConsoleTextAttribute(hStdOut, wColor);
      }
}

char descodificaChar(char * chave, char c){
    if (c >= 65 && c <= 90){
        char retorno = 0;
        for (int i = 0; i < 26; i++){
            if (chave[i] == c)
                retorno = i+65;
        }
        return retorno;
    }
    else return c;
}

void leArq(char* frase,char * entrada){
    FILE* ptr;
    char ch;
    int q=0;
    ptr = fopen(entrada, "r");
    if (NULL == ptr) {
        printf("arquivo nao pode ser aberto \n");
    }
    do {
        ch = fgetc(ptr);
        frase[q] = ch;
        q++;
    } while (ch != EOF);
    frase[q]= '\0';
    fclose(ptr);
}

int menu(){
    printf("Digite o numero de acordo com a opção desejada:\n");
    printf("1. Apresentar o estado atual da criptoanálise;\n");
    printf("2. Fazer análise de frequência no texto criptografado;\n");
    printf("3. Realizar casamento exato de caracteres no texto criptografado;\n");
    printf("4. Realizar casamento aproximado de caracteres no texto parcialmente decifrado;\n");
    printf("5. Alterar chave de criptografia;\n");
    printf("6. Exportar resultado e encerrar o programa.\n");
    int r;
    scanf("  %d",&r);
    return r;
}

char * chaveToString(char * chave){
    char * chaveS = (char *) malloc(sizeof(char)*52);
    strcpy (chaveS, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    char str[2] = "\0";
    for (int i = 0; i < 26; i++){
        if (chave[i] != 0)
            str[0] = chave[i];
        else str[0] = ' ';
        strcat(chaveS, str);
    }
    return chaveS;
}

void mostrarEstado(char * frase, char * chave){
    printf("=== Texto criptografado ===\n");
    printf("%s\n",frase);
    printf("=== Chave ===\n");
    printf("%s\n\n", chaveToString(chave));
    printf("=== Texto parcialmente decifrado ===\n");
    for (int i = 0; i < strlen(frase); i++){
        if(frase[i] == descodificaChar(chave, frase[i]) || descodificaChar(chave, frase[i]) == 0){
            SetColor(7);
            printf("%c", frase[i]);
        }
        else{
            SetColor(2);
            printf("%c", descodificaChar(chave, frase[i]));
        }
    }
    SetColor(7);
    printf("\n");
}

int main()
{
    char frase[2000];
    char entrada[100];
    printf("Digite o arquivo de entrada: ");
    scanf(" %s", entrada);
    leArq(frase, entrada);

    int resposta;
    char chave[26];
    for (int i = 0; i < 26; i++)
        chave[i] = 0;

    do {
        resposta = menu();
        switch (resposta){
            case 1:
                mostrarEstado(frase, chave);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                printf("Informe a letra original, seguida da letra para a qual foi mapeada:\n> ");
                char a, b;
                scanf(" %c %c", &a, &b);
                chave[a-65] = b;
                printf("Registrado: %c -> %c\n", a, b);
                break;
            case 6:
                break;
            default:
                printf("\nOpção inválida\n");
                break;
        }
    }while (resposta != 6);
}
