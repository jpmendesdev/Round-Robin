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


void terminate_process(int id) {
}
// Função para executar o Round Robin
void execute_processes_roundRobin() {
    if (!process_list) {
        terminal_writestring("Nenhum processo existe para escalonar\n");
        return;
    }

    int quantum = rand() % 4; // Quantum definido aleatoriamente entre 1 e 4
    printf("Quantum definido como: %d\n", quantum);

    Process* current = process_list;
    Process* prev = NULL;
    int current_time = 0;

    while (process_list) { // Passa a pecorrer a lista de processos
        current->state = EM_EXECUCAO;
        printf("Tempo %d: Executando processo %d\n", current_time, current->id);

        int tempo_execucao;  
        if(current->time_remaining < quantum){ // essa lógica impede que o  time_remaining fique negativo, ex: caso time_remaining seja = 1 e o quantum = 2, a variável tempo_execucao receberia 1 que seria o mesmo valor do time_remaining e daria 0.
            tempo_execucao = current->time_remaining;
        }else{
            tempo_execucao = quantum;
            //caso contrário a variável continua igual ao quantum
        }
         
        
        
        
        current->time_remaining -= tempo_execucao; // variável tempo_execução     sendo usada de acordo com o que falei em cima
        current_time += tempo_execucao;

        sleep(tempo_execucao);  // Simula a execução

        if (current->time_remaining <= 0) {
            current->state = CONCLUIDO;
            printf("Tempo %d: Processo %d concluído\n", current_time, current->id);

            
            if (current == process_list) { // caso o processo seja o primeiro da lista apontamos a lista para o próximo
                process_list = current->next;
                free(current); // limpa a memória
                current = process_list;
            } else { // aqui é caso o processo esteja no meio ou no fim da lista
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
        } else {
            current->state = PRONTO; //indicando que o processo ainda não terminou, movemos para o final da fila
            printf("Tempo %d: Processo %d - Tempo restante: %d (final da fila)\n", current_time, current->id, current->time_remaining);

        //Lógica para mover o processo para o final da fila
            Process* temp = current;
            if (current == process_list) { // caso o processo seja o primeiro na lista, a lista aponta para o próximo
                process_list = current->next;
                current = process_list;
            } else {
                prev->next = current->next; // se não, o processo atual é pulado
                current = prev->next;
            }

                 temp->next = NULL; // recebe NULL pois será o último na fila
                if (!process_list) {
                process_list = temp;
                prev = NULL;
                current = process_list;
            } else  {
                Process* calda = process_list; // se por acaso a lista ficar vazia, o processo se torna único na lista.
                while (calda->next) calda = calda->next;
                calda->next = temp;
                }
        }

            if (current == process_list) { // atualização do prev, já que o prev é anterior ao current, caso current seja o primeiro na lista, prev vira NULL e caso prev seja NULL rodamos na lista para encontrar ele.
            prev = NULL;
        } else if (prev == NULL) {
            prev = process_list;
            while (prev && prev->next != current) {
                prev = prev->next;
            }
        }
    }

    terminal_writestring("Todos os processos foram executados\n");
}


 
void terminal_initialize(void) {
    terminal_writestring("Inicializando terminal...\n");
}

 
void terminal_writestring(const char* str) {
    printf("%s", str);
}