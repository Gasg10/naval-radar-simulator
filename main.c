/*
 * Gonçalo Gonçalves
 * Numero de aluno: a22405201
 * Radar Náutico - Simulador de Embarcações
 * Linguagens de Programação I - 2024/2025
 *
 * Implementação dos 4 tipos de barco para ultimo digito 1:
 * 1 - ProfPaiMau (nao afunda quando ha colisões)
 * 2 - Cruzador (velocidade dupla quando nao ha barcos perto)
 * 3 - Submarino (fica visivel/invisivel a cada 5 frames)
 * 4 - Pescador (para durante 5 frames em cada 10)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// estrutura que guarda os dados dum barco
// aqui fica a informação que nao muda do barco
typedef struct NoVessel  {
    char nome;                    // letra que identifica o barco
    int tipologia;               // que tipo de barco é (1 a 4
    int frame_inicial;           // em que frame apareceu primeiro
    int velocidade_original[2];  // velocidade inicial [vx, vy]
    int angulo_original;         // angulo inicial em graus
} NoVessel;

// esta estrutura representa um barco num determinado momento
// só aparecem aqui os barcos que estao visiveis no radar
typedef struct EntidadeIED {
    int posicao[2];              // onde esta o barco [x, y]
    int velocidade[2];           // velocidade atual [vx, vy]
    NoVessel *no_nautico;        // aponta para os dados do barco
    struct EntidadeIED *seguinte; // próximo barco na lista
} EntidadeIED;

// guarda tudo sobre um frame da simulação
// cada frame é um segundo da simulaçao
typedef struct BaseDados {
    EntidadeIED *frame_atual;    // lista de barcos visiveis
    struct BaseDados *next;      // proximo frame no historico
    int frame_atual_num;         // numero do frame (quantos segundos)
    int dimensao_grelha[2];      // tamanho da grelha [largura, altura]
} BaseDados;

// declarações das funcoes , organizei por categorias para ficar mais arrumado

// funçoes para criar e libertar memoria
NoVessel *criarBarco(char nome, int tipologia, int frame_atual, int vel_x, int vel_y, int angulo_original);
EntidadeIED *criarEntidade(int posicao_x, int posicao_y, int velocidade_x, int velocidade_y, NoVessel *no_nautico);
BaseDados *criarBase(int largura, int altura);
void adicionarBarco(BaseDados *base, EntidadeIED *entidade);
void libertarEntidade(EntidadeIED *entidade);
void libertarBase(BaseDados *base);


// ler e escrever ficheiros
int lerFicheiro(const char *nome_ficheiro, BaseDados *base);
int escreverFicheiro(const char *nome_ficheiro, BaseDados *base);

// simulacão e movimento
void calcularVelocidade(int angulo, int velocidade, int velocidade_resultante[2]);
BaseDados *atualizarJogo(BaseDados *base_atual, int num_frames);
void moverBarco(EntidadeIED *entidade, EntidadeIED *todas_entidades, BaseDados *base);

// verificar colisões e outras validações
int verificarBatida(EntidadeIED *entidade1, EntidadeIED *entidade2);
int dentroDoMapa(EntidadeIED *entidade, int largura, int altura);
int contarVizinhos(EntidadeIED *entidade, EntidadeIED *lista, int distancia);

// funcoes do menu
BaseDados *voltarAtras(BaseDados *base_inicial, int frame_alvo);
EntidadeIED *procurarBarco(EntidadeIED *lista, char nome);
void mostrarVelocidade(BaseDados *primeiro_frame, BaseDados *frame_atual, char nome_barco);
void preverBatidas(BaseDados *base);
int alterarBarco(BaseDados *base, char nome, int posicao_x, int posicao_y, int angulo, int velocidade, int tipo);


// validacões e input do utilizador
int tipoValido(int tipo);
int lerNumero(const char *mensagem, int min, int max);
char lerLetra(const char *mensagem);
int lerAngulo(const char *mensagem);
void limparBuffer(void);

// funcoes auxiliaress
EntidadeIED *copiarLista(EntidadeIED *original);
NoVessel *copiarBarco(NoVessel *original);
BaseDados *encontrarFrame(BaseDados *base_inicial, int frame_numero);

// esta função limpa o que fica no buffer do teclado
// aprendi isto porque às vezes o scanf deixa lixo e da problemas
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n') {
    }
}

// esta função pede um numero ao utilizador e só aceita se estiver no intervalo certo
// fica num loop até meter algo válido , é muito util para evitar crashes
int lerNumero(const char *mensagem, int min, int max) {
    int valor;
    char buffer[100];
    int resultado;

    while(1) {
        printf("%s", mensagem);
        if(fgets(buffer, sizeof(buffer), stdin) != NULL) {
            resultado = sscanf(buffer, "%d", &valor);
            if(resultado == 1) {
                if(valor >= min && valor <= max) {
                    return valor;
                 } else{
                    printf("Tem que ser entre %d e %d!\n", min, max);
                }
             }else{
                printf("Isso nao e um numero!\n");
            }
        }else {
            printf("Erro a ler input\n");
        }

    }
}

// pede uma letra ao utilizador - uso para o nome dos barcos
char lerLetra(const char *mensagem) {
    char buffer[100];
    while(1) {
        printf("%s", mensagem);
        if(fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if(strlen(buffer) >= 2 && buffer[0] != '\n') {
                char c = buffer[0];
                if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                    return c;
                }
            }
        }
        printf("Entrada invalida. Por favor, introduza uma letra.\n");
    }
}

// so aceita os angulos que o professor disse que eram validos
// 0, 45, 90, 135, 180, 225, 270, 315 - as 8 direções basicas
int lerAngulo(const char *mensagem)  {
    int ang;
    char buf[100];
    int valido = 0;

    while(1){
        printf("%s", mensagem);
        if(fgets(buf, sizeof(buf), stdin) != NULL) {
            if(sscanf(buf, "%d", &ang) == 1){
                // verificar se e um dos angulos validos
                if(ang == 0)
                    valido = 1;
                else if(ang == 45)
                    valido = 1;
                else if(ang == 90)
                    valido = 1;
                else if(ang == 135)
                    valido = 1;
                else if(ang == 180)
                    valido = 1;
                else if(ang == 225)
                    valido = 1;
                else if(ang == 270)
                    valido = 1;
                else if(ang == 315)
                    valido = 1;
                else
                    valido = 0;

                if(valido) {
                    return ang;
                }
            }
        }
        printf("Entrada invalida. Use apenas: 0, 45, 90, 135, 180, 225, 270, 315 graus\n");
    }
}

// como o meu numero acaba em 1, só posso usar os tipos 1, 2, 3 e 4
int tipoValido(int tipo) {
    return (tipo >= 1 && tipo <= 4);
}



// função principal onde tudo começa
int main(int argc, char *argv[]) {
    // verificar se o utilizador passou os argumentos todos
    if(argc != 5) {
        printf("Uso: %s <ficheiro_entrada> <dimensao_grelha> <num_frames> <ficheiro_saida>\n", argv[0]);
        return 1;
    }

    // guardar os argumentos em variaveis para ser mais facil de usar
    char *ficheiro_entrada = argv[1];
    char *dimensao_grelha = argv[2];
    int num_frames = atoi(argv[3]);
    char *ficheiro_saida = argv[4];

    // processar as dimensões da grelha que vem no formato largurax altura
    int largura, altura;
    if(sscanf(dimensao_grelha, "%dx%d", &largura, &altura) != 2) {
        printf("Formato de grelha invalido. Use o formato: larguraxaltura (ex 50x50)\n");
        return 1;
    }

    // verificar se as dimensoes fazem sentido
    if(largura <= 0 || altura <= 0)  {
        printf("As dimensões da grelha devem ser positivas.\n");
        return 1;
    }

    // criar a estrutura inicial para guardar tudo (frame 0)
    BaseDados *base_inicial = criarBase(largura, altura);
    if(base_inicial == NULL) {
        printf("Erro ao criar a base de dados\n");
        return 1;
    }

    // carregar os barcos do ficheiro que o prof deu
    if(lerFicheiro(ficheiro_entrada, base_inicial) != 0){
        libertarBase(base_inicial);
        return 1;
    }

    // se o utilizador pediu para simular logo alguns frames, fazemos isso
    BaseDados *base_atual = base_inicial;
    if(num_frames > 0)  {
        base_atual = atualizarJogo(base_inicial, num_frames);
        printf("Simulação atualizada para o frame %d\n", base_atual->frame_atual_num);

    }

    // agora mostrar o menu para o utilizador escolher o que quer fazer
    int opcao;
    do {
        printf("\n=== MENU DA SIMULACAO ===\n");
        printf("1. Atualizar simulacao\n");
        printf("2. Inserir ou alterar barco\n");
        printf("3. Previsao de colisoes\n");
        printf("4. Rastrear historico reverso\n");
        printf("5. Velocidade media de um barco\n");
        printf("0. Sair\n");

        opcao = lerNumero("Escolha uma opcao: ", 0, 5);

        switch(opcao) {
            case 1: { //  fazer a simulação avançar mais frames
                int frames = lerNumero("Quantos frames deseja avançar? ", 1, 1000) ;
                base_atual = atualizarJogo(base_atual, frames);

                // gravar o resultado automaticamente
                if(escreverFicheiro(ficheiro_saida, base_atual) == 0) {
                    printf("Frame %d guardado com sucesso em %s\n", base_atual->frame_atual_num, ficheiro_saida);
                }
                printf("Simulação atualizada para o frame %d\n", base_atual->frame_atual_num);
                break;
            }

            case 2:{ // adicionar um barco novo ou mudar um que jaa existe
                printf("\n=== Inserir/Alterar Barco ===\n");

                char nome = lerLetra("Nome do barco (uma letra): ");

                //pedir a posição com validação para nao sair da grelha
                printf("Posicao inicial (latitude longitude): ");
                char buffer[100];
                int posicao_x, posicao_y;
                while(1) {
                    if(fgets(buffer, sizeof(buffer), stdin) != NULL) {
                        if(sscanf(buffer, "%d %d", &posicao_x, &posicao_y) == 2){
                            if(posicao_x >= 0 && posicao_x < largura && posicao_y >= 0 && posicao_y < altura) {
                                break;
                            }
                        }
                    }
                    printf("Posição inválida. Use coordenadas validas (0-%d, 0-%d): ", largura-1, altura-1);
                }

                int angulo = lerAngulo("Angulo (múltiplo de 45): ");
                int velocidade = lerNumero("Velocidade: ", 0, 100);
                int tipo = lerNumero("Tipo do barco (1-4): ", 1, 4);

                // tentar inserir ou atualizar o barco
                int resultado = alterarBarco(base_atual, nome, posicao_x, posicao_y, angulo, velocidade, tipo);

                if(resultado == 1) {
                    printf("Barco %c alterado com sucesso.\n", nome);
                } else if(resultado == 0) {
                    printf("Barco %c adicionado com sucesso.\n", nome);
                } else{
                    printf("Erro ao adicionar/atualizar o barco %c.\n", nome);
                }
                break;
            }

            case 3:   //  ver que barcos vão bater uns nos outros
                printf("\n=== Previsão de Colisões ===\n");
                preverBatidas(base_atual);
                break;

            case 4:{ // voltar atrás no tempo
                printf("\n=== Rastrear Historico Reverso ===\n");
                int frames_reverter = lerNumero("Quantos frames deseja voltar? ", 0, base_atual->frame_atual_num);

                int frame_alvo = base_atual->frame_atual_num - frames_reverter;
                base_atual = voltarAtras(base_inicial, frame_alvo);
                printf("Estado do frame %d:\n", base_atual->frame_atual_num) ;

                // mostrar onde estão os barcos nesse momento
                EntidadeIED *entidade = base_atual->frame_atual;
                if(entidade == NULL) {
                    printf("Nenhum barco presente neste frame.\n");
                } else{
                    while(entidade != NULL) {
                        printf("Barco %c: posição (%d,%d), velocidade (%d,%d)\n",
                               entidade->no_nautico->nome,
                               entidade->posicao[0], entidade->posicao[1],
                               entidade->velocidade[0], entidade->velocidade[1]);
                        entidade = entidade->seguinte;
                    }
                }
                break;
            }
            case 5:  {  // calcular a velocidade media dum barco especifico
                printf("\n=== Velocidade Média de um Barco ===\n");
                char nome = lerLetra("Nome do barco (uma letra): ");
                mostrarVelocidade(base_inicial, base_atual, nome);
                break;
            }

            case 0: // sair do programa
                 // gravar o estado final antes de sair
                 if(escreverFicheiro(ficheiro_saida, base_atual) == 0) {
                    printf("Frame %d guardado com sucesso em %s\n", base_atual->frame_atual_num, ficheiro_saida) ;
                }
                printf("A sair do programa...\n");
                break;
        }
    } while(opcao != 0);
    // libertar toda a memoria que aloquei antes de terminar
    libertarBase(base_inicial);
    return 0;
}
// criar uma nova embarcaçao com todos os dados que preciso
NoVessel *criarBarco(char nome, int tipologia, int frame_atual, int vel_x, int vel_y, int angulo_original) {
    NoVessel *novo = (NoVessel *)malloc(sizeof(NoVessel));
    if(novo == NULL)  {
        printf("Erro ao alocar memoria para NoVessel\n");
        return NULL;
    }

    novo->nome = nome;
    novo->tipologia = tipologia;
    novo->frame_inicial = frame_atual;
    novo->velocidade_original[0] = vel_x;
    novo->velocidade_original[1] = vel_y;
    novo->angulo_original = angulo_original;

    return novo;
}

// criar uma entidade que aparece no radar num determinado momento
EntidadeIED *criarEntidade(int posicao_x, int posicao_y, int velocidade_x, int velocidade_y, NoVessel *no_nautico){
    EntidadeIED *nova = (EntidadeIED *)malloc(sizeof(EntidadeIED)) ;
    if  (nova == NULL) {
        printf("Erro ao alocar memória para EntidadeIED\n");
        return NULL;
    }
    nova->posicao[0] = posicao_x;
    nova->posicao[1] = posicao_y;
    nova->velocidade[0] = velocidade_x;
    nova->velocidade[1] = velocidade_y;
    nova->no_nautico = no_nautico;
    nova->seguinte = NULL;


    return nova;
}


// criar a estrutura principal que guarda tudo sobre um frame
BaseDados *criarBase(int largura, int altura)  {
    BaseDados *nova = (BaseDados *)malloc(sizeof(BaseDados));
    if(nova == NULL) {
        printf("Erro ao alocar memória para BaseDados\n");
        return NULL;
    }
    nova->frame_atual = NULL;
    nova->next = NULL;
    nova->frame_atual_num = 0;
    nova->dimensao_grelha[0] = largura;
    nova->dimensao_grelha[1] = altura;
    return nova;
}



// adicionar uma entidade à lista deste frame
// meto sempre no inicio porque é mais rapido que ir ate ao fim
void adicionarBarco(BaseDados *base, EntidadeIED *entidade) {
    if(base == NULL || entidade == NULL) {
        return;
    }

    entidade->seguinte = base->frame_atual;
    base->frame_atual = entidade;
}
// libertar a memoria duma entidade
void libertarEntidade(EntidadeIED *entidade)  {
    if(entidade == NULL) {
        return;
    }
    // libertar primeiro a embarcacao e depois a entidade
    if(entidade->no_nautico != NULL){
        free(entidade->no_nautico);
    }

    free(entidade);

}


//  libertar toda a memoria da simulacao - muito importante para nao ter leaks
void libertarBase(BaseDados *base) {
    if(base == NULL) {
        return;
    }

    BaseDados *atual = base;

    // percorrer todos os frames
    while(atual != NULL) {
        BaseDados *prox = atual->next;

        // libertar todas as entidades deste frame
        EntidadeIED *entidade = atual->frame_atual;
        while(entidade != NULL) {
            EntidadeIED *prox_entidade = entidade->seguinte;
            libertarEntidade(entidade);
            entidade = prox_entidade;
        }

        free(atual);
        atual = prox;
    }
}


// carregar os barcos do ficheiro que o prof nos deu
int lerFicheiro(const char *nome_ficheiro, BaseDados *base) {
    FILE *ficheiro = fopen(nome_ficheiro, "r");
    if(ficheiro == NULL) {
        printf("Erro ao abrir o ficheiro de entrada: %s\n", nome_ficheiro);
        return 1;
    }
    char nome;
    int latitude, longitude, angulo, velocidade, tipo;

    // ler linha por linha do ficheiro
    while(fscanf(ficheiro, " %c %d %d %d %d %d", &nome, &latitude, &longitude, &angulo, &velocidade, &tipo) == 6) {
        // so aceitar os tipos que posso implementar (1,2,3,4))
        if(!tipoValido(tipo)) {
            continue; // ignorar barcos com tipo que nao posso usar
        }
        // verificar se o barco esta dentro da grelha
        if (latitude < 0 || latitude >= base->dimensao_grelha[0] ||
           longitude < 0 || longitude >= base->dimensao_grelha[1]) {
            printf("Aviso: Barco %c na posição (%d,%d) fora da grelha %dx%d - ignorado\n",
                    nome, latitude, longitude,
                    base->dimensao_grelha[0], base->dimensao_grelha[1]);
            continue;
           }

        // converter o angulo e velocidade para vx, vy como o prof explicou
        int velocidade_resultante[2];
        calcularVelocidade(angulo, velocidade, velocidade_resultante);

        // criar a embarcação
        NoVessel *no_nautico = criarBarco(nome, tipo, base->frame_atual_num,
                                           velocidade_resultante[0], velocidade_resultante[1], angulo);
        if(no_nautico == NULL) {
            fclose(ficheiro);
            return 1;
        }

        // criar a entidade que vai aparecer no radar
        EntidadeIED *entidade = criarEntidade(latitude, longitude,
                                                velocidade_resultante[0],
                                                velocidade_resultante[1],
                                                no_nautico);
        if(entidade == NULL)  {
            free(no_nautico);
            fclose(ficheiro);
            return 1;
        }

        adicionarBarco(base, entidade);
    }

    fclose(ficheiro);
    return 0;
}
// gravar o estado atual no ficheiro de saida
int escreverFicheiro(const char *nome_ficheiro, BaseDados *base) {
    FILE *ficheiro = fopen(nome_ficheiro, "w");
    if(ficheiro == NULL) {
        printf("Erro ao abrir o ficheiro de saída: %s\n", nome_ficheiro);
        return 1;
    }

    EntidadeIED *entidade = base->frame_atual;

    while(entidade != NULL) {
        int angulo;
        int vx = entidade->velocidade[0];
        int vy = entidade->velocidade[1];

        // converter de volta para angulo baseado na velocidade
        if(vx == 0 && vy == 0) {
            angulo = entidade->no_nautico->angulo_original;
        } else{
            // estas conversões segem a lógica que o prof explicou
            if(vx == 0 && vy < 0) angulo = 270;
            else if(vx == 0 && vy > 0) angulo = 90;
            else if(vx < 0 && vy == 0) angulo = 180;
            else if(vx > 0 && vy == 0) angulo = 0;
            else if(vx > 0 && vy > 0) angulo = 45;
            else if(vx < 0 && vy > 0) angulo = 135;
            else if(vx < 0 && vy < 0) angulo = 225;
            else if(vx > 0 && vy < 0) angulo = 315;
            else angulo = entidade->no_nautico->angulo_original;
        }

        // calcular a magnitude da velocidade usando pitagoras
        int velocidade = (int)sqrt(vx * vx + vy * vy);

        fprintf(ficheiro, "%c %d %d %d %d %d\n",
                entidade->no_nautico->nome,
                entidade->posicao[0],
                entidade->posicao[1],
                angulo,
                velocidade,
                entidade->no_nautico->tipologia);

        entidade = entidade->seguinte;
    }
    fclose(ficheiro);
    return 0;
}



// esta função converte um angulo em graus para componentes vx e vy
// segue exatamente o que o professor explicou na aula
void calcularVelocidade(int angulo, int velocidade, int velocidade_resultante[2])  {
    // converter angulo para velocidade x e y como o prof explicou
    if(angulo == 0){
        // vai para a direita
        velocidade_resultante[0] = velocidade;
        velocidade_resultante[1] = 0;
    } else if(angulo == 45){
         // vai para nordeste - x positivo, y positivo
        velocidade_resultante[0] = velocidade;
        velocidade_resultante[1] = velocidade;
    }else if(angulo == 90) {
        // vai para baixo
        velocidade_resultante[0] = 0;
        velocidade_resultante[1] = velocidade;
    } else if(angulo == 135) {
        // vai para noroeste
        velocidade_resultante[0] = -velocidade;
        velocidade_resultante[1] = velocidade;
    }else if(angulo == 180)  {
        // vai para a esquerda
        velocidade_resultante[0] = -velocidade;
        velocidade_resultante[1] = 0;
    } else if(angulo == 225) {
         // vai para sudoeste
        velocidade_resultante[0] = -velocidade;
        velocidade_resultante[1] = -velocidade;
    } else if(angulo == 270) {
        // vai para cima
        velocidade_resultante[0] = 0;
        velocidade_resultante[1] = -velocidade;
    } else if(angulo == 315) {
        // vai para sudeste
        velocidade_resultante[0] = velocidade;
        velocidade_resultante[1] = -velocidade;
    } else{
        // se der erro, fica parado
        velocidade_resultante[0] = 0;
        velocidade_resultante[1] = 0;
    }
}


// criar uma copia de uma embarcação - preciso disto para o histórico
NoVessel *copiarBarco(NoVessel *original){
    if(original == NULL) {
        return NULL;
    }
    NoVessel *copia = (NoVessel *)malloc(sizeof(NoVessel));
    if(copia == NULL) {
        return NULL;
    }


    // copiar todos os campos
    copia->nome = original->nome;
    copia->tipologia = original->tipologia;
    copia->frame_inicial = original->frame_inicial;
    copia->velocidade_original[0] = original->velocidade_original[0];
    copia->velocidade_original[1] = original->velocidade_original[1];
    copia->angulo_original = original->angulo_original;
    return copia;
}

// esta funcao nao estou a usar atualmente mas pode ser util no futuro
EntidadeIED *copiarLista(EntidadeIED *original)  {
    if(original == NULL)  {
        return NULL;
    }
    NoVessel *no_nautico_copia = copiarBarco(original->no_nautico);
    if(no_nautico_copia == NULL){
        return NULL;
    }

    EntidadeIED *copia = criarEntidade(original->posicao[0], original->posicao[1],
                                         original->velocidade[0], original->velocidade[1],
                                         no_nautico_copia);
    if(copia == NULL) {
        free(no_nautico_copia);
        return NULL;
    }

    EntidadeIED *atual_original = original->seguinte;
    EntidadeIED *atual_copia = copia;

    while(atual_original != NULL)  {
        no_nautico_copia = copiarBarco(atual_original->no_nautico);
        if(no_nautico_copia == NULL) {
            // se alguma coisa correr mal, limpar tudo o que ja fiz
            EntidadeIED *temp = copia;
            while(temp != NULL){
                EntidadeIED *prox = temp->seguinte;
                libertarEntidade(temp) ;
                temp = prox;
            }
            return NULL;
        }

        EntidadeIED *nova_copia = criarEntidade(atual_original->posicao[0], atual_original->posicao[1],
                                                 atual_original->velocidade[0], atual_original->velocidade[1],
                                                 no_nautico_copia);
        if(nova_copia == NULL) {
            free(no_nautico_copia);
            EntidadeIED *temp = copia;
            while(temp != NULL) {
                EntidadeIED *prox = temp->seguinte;
                libertarEntidade(temp);
                temp = prox;
            }
            return NULL;
        }
        atual_copia->seguinte = nova_copia;
        atual_copia = nova_copia;
        atual_original = atual_original->seguinte;

    }

    return copia;

}

// verificar se duas entidades batem uma na outra (mesma posição)
int verificarBatida(EntidadeIED *entidade1, EntidadeIED *entidade2){
    int x1 = entidade1->posicao[0];
    int y1 = entidade1->posicao[1];
    int x2 = entidade2->posicao[0];
    int y2 = entidade2->posicao[1];
    int mesmoX = 0;
    int mesmoY = 0;

    if(x1 == x2) {
        mesmoX = 1;
    }
    if(y1 == y2) {
        mesmoY = 1;
    }

    if(mesmoX == 1 && mesmoY == 1)  {
        return 1;
    }else{
        return 0;
    }
}

// verificar se uma entidade está dentro dos limites da grelha
int dentroDoMapa(EntidadeIED *entidade, int largura, int altura) {
    return (entidade->posicao[0] >= 0 && entidade->posicao[0] < largura &&
            entidade->posicao[1] >= 0 && entidade->posicao[1] < altura);
}

// contar quantos barcos estão perto de uma entidade - uso isto para os comportamentos especiais
int contarVizinhos(EntidadeIED *entidade, EntidadeIED *lista, int distancia) {
     int contador = 0;
    EntidadeIED *atual = lista;

    while(atual != NULL)  {
        if(atual != entidade) { // nao contar o proprio barco
            int dx = atual->posicao[0] - entidade->posicao[0];
            int dy = atual->posicao[1] - entidade->posicao[1];
            //  calcular valor absoluto manualmente
            if(dx < 0){
                dx = -dx;
            }
            if(dy < 0) {
                dy = -dy;
            }
            // verificar se esta dentro do quadrado de distancia
            if(dx <= distancia && dy <= distancia) {
                contador++;
            }

        }
        atual = atual->seguinte;
    }

    return contador;
}
// esta é a parte mais importante - aqui implemento os comportamentos dos 4 tipos de barco
// para o meu numero que acaba em 1: ProfPaiMau, Cruzador, Submarino, Pescador
void moverBarco(EntidadeIED *entidade, EntidadeIED *todas_entidades, BaseDados *base) {
    if(entidade == NULL || entidade->no_nautico == NULL || base == NULL) {
        return;
    }

    // calcular frames desde que o barco apareceu
    int frames_passados = base->frame_atual_num - entidade->no_nautico->frame_inicial;

    // ver que tipo de barco e e fazer o comportamento
    if(entidade->no_nautico->tipologia == 1) {
        // ProfPaiMau - nao faz nada especial aqui
        // o comportamento e nas colisoes
    } else if(entidade->no_nautico->tipologia == 2) {
        // Cruzador - velocidade dupla se nao ha barcos perto
        int barcos_perto = contarVizinhos(entidade, todas_entidades, 4);
        if(barcos_perto == 0){
            // duplicar velocidade
            entidade->velocidade[0] = entidade->no_nautico->velocidade_original[0] * 2;
            entidade->velocidade[1] = entidade->no_nautico->velocidade_original[1] * 2;
        } else  {
            // velocidade normal
            entidade->velocidade[0] = entidade->no_nautico->velocidade_original[0];
            entidade->velocidade[1] = entidade->no_nautico->velocidade_original[1];
        }
    } else if(entidade->no_nautico->tipologia == 3) {
        // Submarino - aparece e desaparece mas isso e controlado noutro lado
    }  else if(entidade->no_nautico->tipologia == 4) {
        // Pescador - para 5 frames de cada 10
        int resto = frames_passados % 10;
        if(resto < 5){
            // primeiros 5 frames - parado
            entidade->velocidade[0] = 0;
            entidade->velocidade[1] = 0;
        } else{
            // ultimos 5 frames - movimento normal
            entidade->velocidade[0] = entidade->no_nautico->velocidade_original[0];
            entidade->velocidade[1] = entidade->no_nautico->velocidade_original[1];
        }
    } else  {
        // tipo que nao reconheco - usar velocidade original
        entidade->velocidade[0] = entidade->no_nautico->velocidade_original[0];
        entidade->velocidade[1] = entidade->no_nautico->velocidade_original[1];
    }
}


// esta é a função mais complexa - aqui acontece toda a simulação
// movimento, comportamentos especiais, colisões, submarinos que desaparecem, etc.
BaseDados *atualizarJogo(BaseDados *base_atual, int num_frames){
    BaseDados *frame_atual = base_atual;

    // fazer cada frame um por um
    for(int i = 0; i < num_frames; i++)  {
        // criar o próximo frame
        BaseDados *novo_frame = criarBase(frame_atual->dimensao_grelha[0], frame_atual->dimensao_grelha[1]);
        if (novo_frame == NULL){
            return frame_atual; // se correr mal, devolver o que temos
        }

        novo_frame->frame_atual_num = frame_atual->frame_atual_num + 1;

        EntidadeIED *entidade_atual = frame_atual->frame_atual;

        // processar cada barco do frame atual
        while(entidade_atual != NULL) {
            // verificar se o submarino deve aparecer neste frame
            int deve_aparecer = 1;
            if(entidade_atual->no_nautico->tipologia == 3) { // Submarino
                int frame_desde_inicio = novo_frame->frame_atual_num - entidade_atual->no_nautico->frame_inicial;
                deve_aparecer = ((frame_desde_inicio % 10) < 5); // aparece 5, desaparece 5
            }

            if(deve_aparecer) {
                // criar cópia independente da embarcação
                NoVessel *novo_no = copiarBarco(entidade_atual->no_nautico);
                if(novo_no == NULL) {
                    libertarBase(novo_frame);
                    return frame_atual;
                }

                // criar nova entidade para o próximo frame
                EntidadeIED *nova_entidade = criarEntidade(
                    entidade_atual->posicao[0],
                    entidade_atual->posicao[1],
                    entidade_atual->velocidade[0],
                    entidade_atual->velocidade[1],
                    novo_no
                );

                if(nova_entidade == NULL){
                    free(novo_no);
                    libertarBase(novo_frame);
                    return frame_atual;
                }
                // aplicar comportamento especial ANTES de mover
                moverBarco(nova_entidade, frame_atual->frame_atual, novo_frame);

                // agora mover o barco baseado na velocidade
                nova_entidade->posicao[0] += nova_entidade->velocidade[0];
                nova_entidade->posicao[1] += nova_entidade->velocidade[1];

                // so adicionar se ainda estiver dentro da grelha
                if(dentroDoMapa(nova_entidade, novo_frame->dimensao_grelha[0], novo_frame->dimensao_grelha[1])) {
                    adicionarBarco(novo_frame, nova_entidade);
                } else  {
                    // o barco saiu da grelha - remover
                    libertarEntidade(nova_entidade);
                }
            }

            entidade_atual = entidade_atual->seguinte;
        }

        // agora tratar das colisões , esta parte foi complicada de fazer
        EntidadeIED *entidade1 = novo_frame->frame_atual;
        EntidadeIED *anterior1 = NULL;

        while(entidade1 != NULL) {
            EntidadeIED *entidade2 = entidade1->seguinte;
            EntidadeIED *anterior2 = entidade1;
            int entidade1_removida = 0;

            // verificar colisões da entidade1 com todas as que vêm depois
            while(entidade2 != NULL && !entidade1_removida) {
                if(verificarBatida(entidade1, entidade2)) {
                    // determinar quais barcos afundam (ProfPaiMau tipo 1 não afunda)
                    int entidade1_afunda = (entidade1->no_nautico->tipologia != 1);
                    int entidade2_afunda = (entidade2->no_nautico->tipologia != 1);

                    // remover entidade2 se deve afundar
                    if(entidade2_afunda)  {
                        anterior2->seguinte = entidade2->seguinte;
                        libertarEntidade(entidade2);
                        entidade2 = anterior2->seguinte;
                    }else{
                        anterior2 = entidade2;
                        entidade2 = entidade2->seguinte;
                    }

                    // remover entidade1 se deve afundar
                    if(entidade1_afunda) {
                        if(anterior1 == NULL){
                            novo_frame->frame_atual = entidade1->seguinte;
                        }else {
                            anterior1->seguinte = entidade1->seguinte;
                        }
                        libertarEntidade(entidade1);
                        entidade1_removida = 1;
                    }

                } else{
                    // nao ha colisao  logo vai continuar
                    anterior2 = entidade2;
                    entidade2 = entidade2->seguinte;
                }
            }
            // avançar para a proxima entidade1
            if(!entidade1_removida) {
                anterior1 = entidade1;
                entidade1 = entidade1->seguinte;
            } else {
                entidade1 = (anterior1 == NULL) ? novo_frame->frame_atual : anterior1->seguinte;
            }
        }
        // ligar este frame ao historico
        frame_atual->next = novo_frame;
        frame_atual = novo_frame;

        // mostrar progresso ao utilizador
        printf("Frame %d guardado com sucesso em depois.txt\n", frame_atual->frame_atual_num);
    }
    return frame_atual;
}

// adicionar um barco novo ou atualizar um que ja existe
int alterarBarco(BaseDados *base, char nome, int posicao_x, int posicao_y, int angulo, int velocidade, int tipo)   {
    if (base == NULL) {
        return -1;
    }
    // verificar se o tipo é valido para o meu número
    if(!tipoValido(tipo))  {
        return -1;
    }


    // verificar se a posição esta dentro da grelha
    if(posicao_x < 0 || posicao_x >= base->dimensao_grelha[0] ||
       posicao_y < 0 || posicao_y >= base->dimensao_grelha[1]) {
        printf("Erro: Posição (%d,%d) fora da grelha %dx%d\n",
               posicao_x, posicao_y, base->dimensao_grelha[0], base->dimensao_grelha[1]);
        return -1;
    }

    // converter angulo para vx, vy
    int velocidade_resultante[2];
    calcularVelocidade(angulo, velocidade, velocidade_resultante);
    // procurar se o barco ja existe
    EntidadeIED *atual = base->frame_atual;

    while(atual != NULL) {
        if  (atual->no_nautico->nome == nome){
            // barco encontrado - atualizar os seus dados
            atual->posicao[0] = posicao_x;
            atual->posicao[1] = posicao_y;
            atual->velocidade[0] = velocidade_resultante[0];
            atual->velocidade[1] = velocidade_resultante[1];
            atual->no_nautico->tipologia = tipo;
            atual->no_nautico->velocidade_original[0] = velocidade_resultante[0];
            atual->no_nautico->velocidade_original[1] = velocidade_resultante[1];
            atual->no_nautico->angulo_original = angulo;

            return 1; // significa que foi atualizado

        }

        atual = atual->seguinte;
    }

    // se chegou aqui é porque o barco não existe - criar novo
    NoVessel *novo_no = criarBarco(nome, tipo, base->frame_atual_num,
                                    velocidade_resultante[0], velocidade_resultante[1], angulo);
    if  (novo_no == NULL) {
        return -1;
    }

    EntidadeIED *nova_entidade = criarEntidade(posicao_x, posicao_y,
                                               velocidade_resultante[0],
                                               velocidade_resultante[1],
                                               novo_no) ;
    if(nova_entidade == NULL){
        free(novo_no);
        return -1;
    }

    adicionarBarco(base, nova_entidade);
    return 0; // significa que foi adicionado
}


// prever colisoes futuras , simulo o movimento para ver onde vão bater
void preverBatidas(BaseDados *base) {
    if(base == NULL || base->frame_atual == NULL) {
        printf("Total de colisões previstas: 0\n");
        return;
    }

    int total_colisoes = 0;
    EntidadeIED *entidade1 = base->frame_atual;

    // verificarr cada par de barcos
    while(entidade1 != NULL) {
        EntidadeIED *entidade2 = entidade1->seguinte;


        while(entidade2 != NULL) {
            // simular movimento futuro
            int pos1_x = entidade1->posicao[0];
            int pos1_y = entidade1->posicao[1];
            int pos2_x = entidade2->posicao[0];
            int pos2_y = entidade2->posicao[1];
            int frame_colisao = -1;
            int x_colisao = -1;
            int y_colisao = -1;

            // simular até 100 frames no futuro
            for  (int i = 1; i <= 100; i++) {
                pos1_x += entidade1->velocidade[0];
                pos1_y += entidade1->velocidade[1];
                pos2_x += entidade2->velocidade[0];
                pos2_y += entidade2->velocidade[1];

                // verificar se vão estar na mesma posicao
                if(pos1_x == pos2_x && pos1_y == pos2_y) {
                    // verificar se a colisao acontece dentro da grelha
                     if(pos1_x >= 0 && pos1_x < base->dimensao_grelha[0] &&
                       pos1_y >= 0 && pos1_y < base->dimensao_grelha[1]) {
                        frame_colisao = base->frame_atual_num + i;
                        x_colisao = pos1_x;
                        y_colisao = pos1_y;
                        break;
                    }
                }
            }

            // se encontrou uma colisão, mostrar a informação
            if  (frame_colisao != -1){
                total_colisoes++;
                printf("Colisão prevista entre barcos %c e %c:\n",
                       entidade1->no_nautico->nome, entidade2->no_nautico->nome);
                printf(" Posicão prevista da colisao: (%d,%d)\n", x_colisao, y_colisao);
            }
            entidade2 = entidade2->seguinte;
        }

        entidade1 = entidade1->seguinte;

    }
    printf("Total de colisoes previstas: %d\n", total_colisoes);
}

// procurar um frame específico no histórico
BaseDados *encontrarFrame(BaseDados *base_inicial, int frame_numero)   {
    BaseDados *atual = base_inicial;

    while(atual != NULL) {
        if(atual->frame_atual_num == frame_numero) {
            return atual;
        }
        atual = atual->next;
    }

    return NULL; // não encontrado
}

// voltar atras no tempo para um frame anterior
BaseDados *voltarAtras(BaseDados *base_inicial, int frame_alvo){
    if (base_inicial == NULL || frame_alvo < 0) {
        return base_inicial;
    }

    BaseDados *frame_encontrado = encontrarFrame(base_inicial, frame_alvo);

    if(frame_encontrado != NULL) {
        return frame_encontrado;
    }

    // se não encontrar o frame, devolver o inicial
    return base_inicial;
}
// calcular estatísticas de movimento de um barco específico
void mostrarVelocidade(BaseDados *primeiro_frame, BaseDados *frame_atual, char nome_barco)  {
    if (primeiro_frame == NULL || frame_atual == NULL) {
        printf("Erro: Frames invalidos\n");
        return;

    }

    // procurar o barco no frame inicial
    EntidadeIED *barco_inicial = procurarBarco(primeiro_frame->frame_atual, nome_barco);

    if(barco_inicial == NULL)  {
        printf("Barco %c não encontrado no frame inicial\n", nome_barco);
        return;
    }

    // procurar o barco no frame atual
    EntidadeIED *barco_atual = procurarBarco(frame_atual->frame_atual, nome_barco);

    if(barco_atual == NULL){
        printf("Barco %c não encontrado no frame atual. Pode ter afundado ou saído da grelha.\n", nome_barco);
        return;
     }

    // calcular distancia percorrida usando teorema de pitagorass
    int dx = barco_atual->posicao[0] - barco_inicial->posicao[0];
    int dy = barco_atual->posicao[1] - barco_inicial->posicao[1];
    double distancia = sqrt(dx * dx + dy * dy);
    // calcular velocidade media
    int num_frames = frame_atual->frame_atual_num - primeiro_frame->frame_atual_num;
    double velocidade_media = (num_frames > 0) ? distancia / num_frames : 0;

    // mostrar as estatísticas ao utilizador
    printf("Estatisticas do barco %c:\n", nome_barco);
    printf(" Posição inicial: (%d,%d)\n", barco_inicial->posicao[0], barco_inicial->posicao[1]);
    printf(" Posição atual: (%d,%d)\n", barco_atual->posicao[0], barco_atual->posicao[1]);
    printf(" Distância percorrida: %.2f casas\n", distancia);
    printf(" Número de frames: %d\n", num_frames);
    printf(" Velocidade média: %.2f casas/frame\n", velocidade_media);

    // mostrar descrição do tipo de barco que implementei
    switch(barco_atual->no_nautico->tipologia){
        case 1:
            printf(" Tipo de barco: %d - ProfPaiMau (não afunda em colisoes)\n", barco_atual->no_nautico->tipologia);
            break;
        case 2:
            printf(" Tipo de barco: %d - Cruzador (duplica velocidade se não há barcos proximos)\n", barco_atual->no_nautico->tipologia);
            break;
        case 3:
            printf(" Tipo de barco: %d - Submarino (aparece e desaparece de 5 em 5 frames)\n", barco_atual->no_nautico->tipologia);
            break;
        case 4:
            printf(" Tipo de barco: %d - Pescador (fica parado 5 frames a cada 10)\n", barco_atual->no_nautico->tipologia);
            break;
        default:
            printf(" Tipo de barco: %d (não implementado para último dígito 1)\n", barco_atual->no_nautico->tipologia);
    }
}


// procurar uma entidade pelo nome na lista
EntidadeIED *procurarBarco(EntidadeIED *lista, char nome)  {
    EntidadeIED *atual = lista;
    while(atual != NULL) {
        if(atual->no_nautico->nome == nome) {
            return atual;
        }
        atual = atual->seguinte;
    }
    return NULL; // naao encontrado
}