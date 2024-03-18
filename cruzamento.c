//////////////////////////////////////////////////////////////////////////////////////////////////
//            Aluna: Leticia Rebecca Medeiros de Lucena                                         //
//            Disciplina: Sistemas em Tempo Real                                                //
//            Projeto 1 - 2023.2                                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_TRENS 10 //Quantidade de trens gerada

//Direção do trem no cruzamento
#define A1_TO_A2 0
#define A1_TO_B2 1
#define B1_TO_A2 2
#define B1_TO_B2 3                 

//Criação da struct com informações de prioridade e direção do trem
typedef struct{
    pthread_t thread;
    int prioridade;
    int direcao;
}Trem;

sem_t semaforo_cruzamento;
sem_t semaforo_prioridade;

//Função do fluxo de trens no cruzamento
void *funcao_fluxo(void *arg){
    Trem *trem = (Trem*)arg;
    
    printf("Trem com prioridade %d e direcao %d se aproximando do cruzamento.\n", trem->prioridade, trem->direcao);
    
    sem_wait(&semaforo_prioridade);
    sem_wait(&semaforo_cruzamento);
  
    printf("Trem com prioridade %d e direcao %d cruzando o cruzamento.\n", trem->prioridade, trem->direcao);
    sem_post(&semaforo_cruzamento);
    
    sleep(1);    //Aguarda um tempo para que outros trens cheguem ao cruzamento
    sem_post(&semaforo_cruzamento);
    
    printf("Trem com prioridade %d e direcao %d saiu do cruzamento.\n", trem->prioridade, trem->direcao);
    
    sem_post(&semaforo_prioridade);
    
    pthread_exit(NULL);
}

int main() {
  
    //Inicialização do semáforo
    sem_init(&semaforo_cruzamento, 0, 1);
    sem_init(&semaforo_prioridade, 0, 1);
    
    //Geração e inicialização dos trens
    Trem trens[NUM_TRENS];
    for (int i = 0; i < NUM_TRENS; i++) {
        trens[i].prioridade = rand() % 3;            //Definição de prioridade aleatória (0: baixa, 1: média, 2: alta)
        int direcao_aleatoria = rand() % 4;          // Definição de direção aleatória (entre 0 e 3 - definição própria, legenda no início do código)
                                                           
        switch (direcao_aleatoria){                  // Mapeamento da direção aleatória para as direções permitidas
            case 0:
            case 1:
                trens[i].direcao = A1_TO_A2;
                break;
            case 2:
                trens[i].direcao = B1_TO_A2;
                break;
            case 3:
                trens[i].direcao = B1_TO_B2;
                break;
        }
      
        pthread_create(&trens[i].thread, NULL, funcao_fluxo, (void *)&trens[i]);
        sleep(rand() % 3);                           //Intervalo aleatório para a chegada de um novo trem (entre 0 e 2seg)
    }
    
    // Espera a criação das threads dos trens terminar
    for (int i = 0; i < NUM_TRENS; i++){
        pthread_join(trens[i].thread, NULL);
    }

    sem_destroy(&semaforo_cruzamento);
    sem_destroy(&semaforo_prioridade);
  
    return 0;
}
