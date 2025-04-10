#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

// Definição dos estados do processo
typedef enum {
    PRONTO,
    EM_EXECUCAO,
    BLOQUEADO,
    CONCLUIDO
} ProcessState;

// Estrutura para representar um processo (PCB)
typedef struct Process {
    int id;
    int quantum;
    int time_remaining;
    ProcessState state;
    struct Process* next;
} Process;

//Cabeça da lista de processos
Process* process_list = NULL;
//id do processo
int next_process_id = 1;

// Funções auxiliares para manipulação de tela
void terminal_initialize(void);
void terminal_writestring(const char* str);


// Funções do gerenciamento de processos
void create_process(void);
void list_processes(void);
void terminate_process(int id);
void execute_processes_fcfs(void);
void execute_processes_roundRobin();



// Função principal do kernel
int main(void) {
    int opcao;
    terminal_initialize();

    // Loop principal do kernel
    while (1) {
        // Verifica processos pendentes antes de continuar
        terminal_writestring("\nKernel Simples\n");
        terminal_writestring("1. Executar Processos (Round Robin)\n");
        terminal_writestring("2. Criar Processo\n");
        terminal_writestring("3. Listar Processos\n");
        terminal_writestring("4. Terminar Processo\n");
        terminal_writestring("5. Sair\n");
        terminal_writestring("Escolha uma opção: ");

        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                execute_processes_roundRobin();
                break;
            case 2:
                create_process();
                break;
            case 3:
                list_processes();
                break;
            case 4:
                terminal_writestring("Digite o ID do processo a ser terminado: ");
                int id;
                if (scanf("%d", &id) == 1) {
                    terminate_process(id);
                }
                break;
            case 5:
                 terminal_writestring("Sistema finalizado.\n");
                exit(0);
                break;
            default:
                terminal_writestring("Opção inválida!\n");
        }

        sleep(1);  // Simula tempo de processamento
    }

    return 0;
}



// Função para criar um novo processo
void create_process(void) {
    Process* new_process = (Process*)malloc(sizeof(Process));
    
    if(!new_process){
        terminal_writestring("Erro: Falha ao criar processo\n");
        return;
    }
    
    new_process->id = next_process_id++;
    new_process->time_remaining = rand() % 10+1; //criacao de tempo aleatorio entre 1 e 10
    new_process->state = PRONTO;
    new_process->next = NULL;
    
    if(!process_list){
        process_list = new_process;
    }else{
        Process* temp = process_list;
        while(temp->next){
            temp=temp->next;
        }
        temp->next = new_process;
    }
    terminal_writestring("Processo criado com sucesso");
}

// Função para listar os processos ativos
void list_processes(void) {
    Process* current = process_list;
    if(!current){
      terminal_writestring("Nenhum processo foi criado ainda");
      return;
    }
    terminal_writestring("Lista de Processos \n");
    while(current){
        printf("ID: %d | ESTADO: %s | TEMPO RESTANTE: %d \n",
        current->id,
        (current->state == PRONTO) ? "PRONTO":
        (current->state == EM_EXECUCAO) ? "EM_EXECUCAO":
        (current->state == BLOQUEADO) ? "BLOQUEADO":
        "CONCLUIDO",
        current->time_remaining);
        
        current=current->next;
    }
}

// Função para terminar um processo
void terminate_process(int id) {
}
// Função para executar o Round Robinho
void execute_processes_roundRobin(){
    int quantum = 0;// Inicializo o quantum como 0
    if(!process_list){                          // o quantum, com um valor fixo decrescerá na quantidade de tempo restante de cada processo em execução
        terminal_writestring("Nenhum processo existe para escalonar");
        return;
      }
    terminal_writestring("Iniciando escalonamento\n");
    Process* current = process_list;
    Process* prev = NULL;
    int current_time=0;
    quantum = rand() % 4; //Defini a criação do quantum como um random
    printf("Quantum definido como: %d\n",quantum);
    while(current){
        //Mudar o estado para EM_EXECUCAO
        current->state = EM_EXECUCAO;
        printf("Tempo %d: Processo %d em execução\n", current_time, current->id);
        
        //SIMULAR A EXECUCAO ATÉ A CONCLUSÃO DO PROCESSO
        while(current->time_remaining>0){ 
            sleep(1);
            current_time++;
            
            if(current->time_remaining < quantum){ // Caso o tempo restante seja menor que o quantum ex: quantum 2, time_remaining = 1, para que ele não retorne o time_remaining negativo ele recebe 0.
                current->time_remaining = 0;
            }else{
                current->time_remaining -= quantum;  //caso não, ele atualiza o time_remanining, decrescendo de acordo com o quantum
            }
            
          //  a cada execução do processo, o quantum será decrescido.
            printf("Tempo %d: Processo %d - Tempo restante: %d \n", current_time, current->id, current->time_remaining); //Printar as informações já com o quantum
        }
        
        current->state = CONCLUIDO;
        printf("Tempo %d: Processo %d concluído\n", current_time, current->id);
        //remover o processo concluido da lista
        if(prev){
            prev->next = current->next;
        } else{
            process_list = current->next;
        }
        free(current);
        current = (prev) ? prev->next: process_list;
    }
    terminal_writestring("Todos os processos foram executados");
}


// Função para inicializar o terminal
void terminal_initialize(void) {
    terminal_writestring("Inicializando terminal...\n");
}

// Função para escrever no terminal
void terminal_writestring(const char* str) {
    printf("%s", str);
}