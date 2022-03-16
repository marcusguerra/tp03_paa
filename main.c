#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    fclose(ptr);
}

int main()
{
    char frase[2000];
    char entrada[100] = "D:\\texto.txt";
    leArq(frase, entrada);
    printf("%s\n",frase);
}