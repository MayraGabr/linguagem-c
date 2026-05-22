#include <stdio.h>
#include <stdlib.h> //gerar aleatorios
#include <pthread.h>
#include <unistd.h> //chamadas de sistema

#define quant 5
#define PENSANDO 0
#define FAMINTO 1
#define COMENDO 2

int estado[quant];

pthread_mutex_t mutex;
pthread_cond_t cond[quant];

// filósofo laterais
int esquerda(int i) { return (i+quant-1)%quant;}
int direita(int i) {return (i+1)%quant;}

void mostrar_tela() {
    system("clear"); 

    printf("===========================\n\n");

    for (int i = 0; i < quant; i++) {
        printf("Filósofo %d: ", i + 1);

        if (estado[i] == PENSANDO)
            printf("PENSANDO\n");
        else if (estado[i] == FAMINTO)
            printf("FAMINTO\n");
        else if (estado[i] == COMENDO)
            printf("COMENDO\n");
    }

    printf("\n===========================\n");
    fflush(stdout);
}

// testa se o filósofo pode comer
void testar_comer(int i) {// se o filosofo i estiver faminto e seus laterais nao comendo

    if (estado[i] == FAMINTO && estado[esquerda(i)] != COMENDO && estado[direita(i)] != COMENDO) {
        estado[i] = COMENDO;
        pthread_cond_signal(&cond[i]); //acorda a thread
    }
}

void pegar_garfos(int i) {

    pthread_mutex_lock(&mutex);//regiao critica

    estado[i] = FAMINTO; 
    mostrar_tela(); 

    testar_comer(i); //tentar comer = tentar pegar os garfos

    while (estado[i] != COMENDO) { //Aguarda "dormindo" o sinal
        pthread_cond_wait(&cond[i], &mutex);
    }

    mostrar_tela(); 

    pthread_mutex_unlock(&mutex);// libera, fim da regiao critica
}

void devolver_garfos(int i) {

    pthread_mutex_lock(&mutex);// regiao critica

    estado[i] = PENSANDO;
    mostrar_tela(); 

    testar_comer(esquerda(i)); // liberar os vizinhos leterais
    testar_comer(direita(i));

    pthread_mutex_unlock(&mutex);// fim da regiao critica
}

// função da thread
void* filosofo(void* num) {

    int i = *(int*)num;//transforma ponteiro em inteiro

    while (1) {
        sleep(rand() % 10);       //pensando
        pegar_garfos(i);         //tentar pegar o garfo
        sleep(rand() % 10);      //comendo
        devolver_garfos(i);      //pensando
    }
    return NULL;
}

int main() {

    pthread_t threads[quant];//vetor para as threads
    int nome_fil[quant];     //numero de cada filosofo

    pthread_mutex_init(&mutex, NULL);//cria mutex

    for (int i = 0; i < quant; i++) {//variaveis de condiçao
        pthread_cond_init(&cond[i], NULL); 
        estado[i] = PENSANDO;
    }

    mostrar_tela(); 

    // cria as threads
    for (int i = 0; i < quant; i++) {
        nome_fil[i] = i;
        pthread_create(&threads[i], NULL, filosofo, &nome_fil[i]);
    }

    // espera todas as threads terminarem
    for (int i = 0; i < quant; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}