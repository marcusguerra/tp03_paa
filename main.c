#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void SetColor(int ForgC){ //modifica a cor do texto
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

char descodificaChar(char * chave, char c){ //descodifica um char com base numa determinada chave
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

int menu(){ //printa o menu e retorna a opção escolhida
    printf("Digite o numero de acordo com a opcao desejada:\n");
    printf("1. Apresentar o estado atual da criptoanalise;\n");
    printf("2. Fazer analise de frequencia no texto criptografado;\n");
    printf("3. Realizar casamento exato de caracteres no texto criptografado;\n");
    printf("4. Realizar casamento aproximado de caracteres no texto parcialmente decifrado;\n");
    printf("5. Alterar chave de criptografia;\n");
    printf("6. Exportar resultado e encerrar o programa.\n");
    int r;
    scanf("  %d",&r);
    return r;
}

char * chaveToString(char * chave){ //transforma a chave numa string
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

void mostrarEstado(char * frase, char * chave){ //mostra o estado atual
    printf("=== Texto criptografado ===\n");
    printf("%s\n",frase); //printa o texto
    printf("=== Chave ===\n");
    printf("%s\n\n", chaveToString(chave)); //printa a chave
    printf("=== Texto parcialmente decifrado ===\n");
    for (int i = 0; i < strlen(frase); i++){ //printa o texto parcialmente decifrado(partes decifradas sao mudadas para a coloracao verde)
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

void mostrarFrequencia(char * frase){
    double freq[] = {14.63,1.04,3.88,4.99,12.57,1.02,1.3,1.28,6.18,0.4,0.02,2.87,4.74,5.05,10.73,2.52,1.2,6.53,7.81,4.34,4.63,1.67,0.01,0.21,0.01,0.47}; //frequencia das palavras em portugues

    int qnts[26];
    int total = 0;
    for (int i = 0; i < 26; i++)
        qnts[i] = 0;

    for (int i = 0; i < strlen(frase); i++){ //soma a quantidade de cada letra
        if (frase[i]-65 >= 0 && frase[i]-65 < 26)
            qnts[frase[i]-65] += 1;
        total += 1;
    }

    printf("Letra, Cont., Freq., No portugues: Letra, Freq.\n");
    for (int i = 0; i < 26; i++){ //mostra a tabela com cada letra e suas frequencias
        int mi = 0;
        int mi2 = 0;
        for (int j = 1; j < 26; j++){
            if(qnts[mi] < qnts[j]){
                mi = j;
            }
            if(freq[mi2] < freq[j]){
                mi2 = j;
            }
        }
        printf("%c       %3d   %5.2lf%%                 %c   %5.2lf%%\n",mi+65,qnts[mi],((double)qnts[mi]*100)/total,mi2+65,freq[mi2]);
        qnts[mi] = -1;
        freq[mi2] = -1;
    }
}

void shifAndPre(int mask[26], char * padrao){ //realiza o pre processamento das funções relativas a shift and
    for (int i = 0; i < 26; i++)
        mask[i] = 0;
    for (int i = 0; i < strlen(padrao); i++) //inicializa a mascara com seus valores adequados
        mask[padrao[i]-65] += pow(2,strlen(padrao) - i - 1);
}

int shiftAnd(char * frase, char * padrao){ //detectar padrao com shift and

    int mask[26];

    shifAndPre(mask, padrao);

    int qnt = 0;
    int state = 0;
    for (int i = 0; i < strlen(frase); i++){ //cada passo do algoritmo shift and
        if(frase[i] >= 65 && frase[i] < 90){ //detecta se e letra
            state = state>>1; //shift
            state += pow(2,strlen(padrao)-1); //ligar o primeiro bit

            state = state & mask[frase[i]-65]; //and com a mascara

            if (state % 2 == 1) //verifica se o ultimo bit esta ligado
                qnt += 1;
        }
        else
            state = 0;
    }
    return qnt; //retorna a quantidade de vezes que encontrou o padrao
}

int shiftAndApx(char * frase, char * padrao, int tolerancia, char * chave){

    //inicialização da mascara
    int mask[26];
    shifAndPre(mask, padrao);

    //tradução do texto
    char texto[2000];
    for (int i = 0; i < strlen(frase); i++){
        if(frase[i] == descodificaChar(chave, frase[i]) || descodificaChar(chave, frase[i]) == 0){texto[i] = frase[i];}
        else{texto[i] = descodificaChar(chave, frase[i]);}}

    //definição de variáveis
    int qnt = 0, state = 0, tol = tolerancia, d = 1;

    //shift and
    for (int i = 0; i < strlen(texto); i++){
        if(texto[i] >= 65 && texto[i] < 90){ //detecta se é letra
            state = state>>1; //shift
            state += pow(2,strlen(padrao)-1); //liga o primeiro bit

            state = state & mask[texto[i]-65]; //and com a mascara

            //tolerancia de erros
            if (state == 0){
                if (tol != 0){
                    state += pow(2,strlen(padrao) - d);//passa para o proximo estagio
                    tol --;}}

            d++;

            if (state % 2 == 1){//verifica se o ultimo bit esta ligado e imprime a palavra e sua localização
                qnt += 1;
                printf("@[%d, %d):", i-strlen(padrao), i);
                for (int j = strlen(padrao)-1; j > -1; --j) {
                    printf("%c", texto[i-j]);}
                printf("\n");}

        }
        else{
            state = 0;
            tol = tolerancia;
            d = 1;}

    }
    return qnt; //retorna a quantidade de vezes que encontrou o padrao
}

int main()
{
    char res[100];
    char res1[100];
    char frase[2000];
    char entrada[100];
    printf("Digite o arquivo de entrada: \n");
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
                mostrarFrequencia(frase);
                break;
            case 3:
                printf("Qual o padrao utilizado?\n> ");
                char padrao[100];
                scanf(" %s", padrao);
                printf("Ocorrencias: %d\n", shiftAnd(frase,padrao));
                break;
            case 4:
                printf("Qual o padrao e a tolerancia utilizados?\n> ");
                int tolerancia;
                scanf(" %s %d", padrao, &tolerancia);
                printf("\nOcorrencias: %d\n", shiftAndApx(frase,padrao, tolerancia, chave));
                break;
            case 5:
                printf("Informe a letra original, seguida da letra para a qual foi mapeada:\n> ");
                char a, b;
                scanf(" %c %c", &a, &b);
                chave[a-65] = b;
                printf("Registrado: %c -> %c\n", a, b);
                break;
            case 6:
                printf("Informe o diretorio do arquivo para ser escrita a chave de criptografia:\n");
                scanf("%s", res);
                FILE *fp1 = fopen(res,"w");
                fwrite(chave, 1 , sizeof(chave) , fp1);
                printf("Informe o diretorio do arquivo para ser escrita a criptoanalise:\n");
                scanf("%s", res1);
                FILE *fp = fopen(res1,"w");
                fclose(fp);

                fp = fopen(res1,"a+");

                for (int i = 0; i < strlen(frase); i++){
                    if(frase[i] == descodificaChar(chave, frase[i]) || descodificaChar(chave, frase[i]) == 0){
                        fwrite(&frase[i] , 1 , sizeof(char) , fp );
                    }
                    else{
                        char x;
                        x = descodificaChar(chave, frase[i]);
                        fwrite(&x , 1 , sizeof(char) , fp );
                    }
                }
                fclose(fp1);
                fclose(fp);
                break;

            default:
                printf("\nOpcao invalida\n");
                break;
        }
    }while (resposta != 6);
}
