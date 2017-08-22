// variaveis globais
#include "util.h"


int numCliente;
int nClientes, capacidadeSala, probDesisteEspera, freqClientes;
int sockfd, dim_serv;
    struct sockaddr_un end_serv;
    
//variaveis de controlo
int salaVIP_lugares = 0;
int TAM_vip;
int numCliPrio = 0;
int numCliNorm = 0;

//semaforos
sem_t sala0;
sem_t semCliVIP;
sem_t semCliNorm;
//sem_t salaVIP;

//trincos
pthread_mutex_t trincoC; //trinco para cliente


//tempo
time_t start;

#define CRIACAO 1
#define ENTRADA 2
#define DESISTENCIA 3
#define SAIDA 4
#define ESPERA 5
#define TERMINAR 6

//função que le ficheiro
void leitura()
{
    FILE* config;
	
    config = fopen("configSim.txt", "r");
    
    if(config != NULL) {
        char linha[50];
        char* valor;
        while(fgets(linha, sizeof(linha), config) != NULL) {
            valor = strtok(linha, ":");
            if(strcmp(valor, "N_CLIENTES") == 0) {
                valor = strtok(NULL, ":");
                nClientes = atoi(valor);
            }
            if(strcmp(valor, "CAPACIDADE_SALA") == 0) {
                valor = strtok(NULL, ":");
                capacidadeSala = atoi(valor);
            }
            if(strcmp(valor, "PROB_DESISTE_FILA") == 0) {
                valor = strtok(NULL, ":");
                probDesisteEspera = atoi(valor);
            }
            if(strcmp(valor, "FREQ_CLIENTES") == 0) {
                valor = strtok(NULL, ":");
                freqClientes = atoi(valor);
            }
            }
    } 
    else 
    {
        printf("ERRO AO ABRIR FICHEIRO\n");
    }
    fclose(config);
	
}
    
    
void envia_recebe_stream (FILE *fp, int sockfd, int a, int id)
{
    int n;
    char buffer[MAXLINHA+1];
    
    if((fgets(buffer, MAXLINHA, fp) == NULL) && ferror(fp))
        perror("erro cliente ao ler input");
    
    n = strlen(buffer) + 1;
    
    if (a == 1)
    {
        
        if(write(sockfd, buffer, n) != n)
            perror("erro cliente no write");
    }
    
    if(write(sockfd, buffer, n) != n)
        perror("erro cliente no write");

}

void envia_stream(char buff[], int sockfdes)
{
    if(write(sockfdes, buff, strlen(buff)+1) == -1)
    {
        perror("ERROR: envia_stream: simulador");
    }
}

void prepara_mensg(int tipo, int id, int tempo, int sala)
{
    char buff[5];
    buff[1] = id;
    buff[3] = sala; 
    switch(tipo)
    {
        case CRIACAO:
            buff[0] = 'A';
            buff[2] = tempo;
            envia_stream(buff, sockfd);
            break;
        case ENTRADA:
            buff[0] = 'B';
            buff[2] = tempo;
            envia_stream(buff, sockfd);
            break;
        case DESISTENCIA:
            buff[0] = 'C';
            buff[2] = tempo;
            envia_stream(buff, sockfd);
            break;
        case SAIDA:
            buff[0] = 'D';
            buff[2] = tempo;
            envia_stream(buff, sockfd);
            break;
        case ESPERA:
            buff[0] = 'E';
            buff[2] = tempo;//hora

            envia_stream(buff, sockfd);
            break;
        case TERMINAR:
            buff[0] = 'F';
            buff[2] = tempo;
            envia_stream(buff, sockfd);
            break;
        default:
            printf("erro evento desconhecido de cliente");
    }
}

void *cliente(void *ptr)
{
    int hora_criacao;
    int clienteID;
    int salaID = rand()%2;
    int cliente_prio = rand()%2 +1;
    int tempo_espera;
    int probDesistencia = rand()%100 + 1;
    time_t startC;
    startC = time(0);
    hora_criacao = time(0) - start;
    
    pthread_mutex_lock(&trincoC);
    
    clienteID = numCliente++;
    
    pthread_mutex_unlock(&trincoC);
    
    prepara_mensg(CRIACAO, clienteID, hora_criacao, salaID);
    printf("\ncliente nº: %d, sala escolhida: %d sua prioridade e:%d!\n", clienteID, salaID, cliente_prio);
    printf("tempo quando chegou: %d\n", hora_criacao);


    if(salaID == 0)
    {
        tempo_espera = time(0);//variavel com o valor com o tempo que esta na fila a espera
        sem_wait(&sala0);

        
        if(probDesistencia < probDesisteEspera && (int)(time(0) - tempo_espera) > 5)
        {
            printf("o cliente %d DESISTIU!\n", clienteID);
            usleep(5000);//sleep critico
            
            pthread_mutex_lock(&trincoC);
            prepara_mensg(DESISTENCIA, clienteID, (int)(time(0) - tempo_espera), salaID);
            pthread_mutex_unlock(&trincoC);
            
            sem_post(&sala0);
            
            return NULL;
        }
        usleep(5000);//SLEEP CRITICO
        
        pthread_mutex_lock(&trincoC);//TRINCO PARA EXPERIMENTARlol
        prepara_mensg(ENTRADA, clienteID, (int)(time(0) - startC), salaID);//CUIDADE COM A POSICAO DESTA LINHA SE FOR PARA METER DESISTENCIAS
        pthread_mutex_unlock(&trincoC);
        
        
        printf("o cliente %d escolheu sala0\n", clienteID);
        
        printf("acabou de entrar o cliente %d\n", clienteID);
        

        usleep(30000000);//fica na disco 20 seg+/-
 
        printf("vai sair o cliente %d da sala0 ao %d min\n", clienteID, (int)(time(0) - start));
        

        sem_post(&sala0);
        
        pthread_mutex_lock(&trincoC);//TRINCO PARA EXPERIMENTAR
        prepara_mensg(SAIDA, clienteID, (int)(time(0) - start), salaID);
        pthread_mutex_unlock(&trincoC);

        
    }
    else//escolha da sala vip
    {

        pthread_mutex_lock(&trincoC);
        if(salaVIP_lugares >= TAM_vip)//ter em atençao se nao é ==
        {
//             if(salaVIP_lugares > 5)
//             {
//                 pthread_mutex_unlock(&trincoC);
//                 printf("o cliente %d DESISTIU, apos %d min a ESPERA\n", clienteID, tempo_espera);//valor de tempo_espera errado
//                 prepara_mensg(DESISTENCIA, clienteID, (int)(time(0) - startC));
//                 
//                 //printf("antes_return_desiste cliente %d\n", clienteID);
//                 return NULL;
//             }
            if(cliente_prio == 2)
            {
                numCliPrio++;
                
                pthread_mutex_unlock(&trincoC);
                
                sem_wait(&semCliVIP);
                
                pthread_mutex_lock(&trincoC);
            }
            else
            {
                numCliNorm++;
                pthread_mutex_unlock(&trincoC);
                
                sem_wait(&semCliNorm);
                
                pthread_mutex_lock(&trincoC);
            }
        }

        salaVIP_lugares++;
        
        pthread_mutex_unlock(&trincoC);
        
        usleep(5000);//sleep CRITICO para poder enviar o prepara_mensg
        
        pthread_mutex_lock(&trincoC);//TRINCO PARA EXPERIMENTAR
        prepara_mensg(ENTRADA, clienteID, (int)(time(0) - startC), salaID);
        pthread_mutex_unlock(&trincoC);
        
        printf("ENTROU o cliente %d PRIO:%d na salaVIP\n", clienteID, cliente_prio);
        
        usleep(30000000);//fica na sala VIP 15 seg+/- lol
        
        pthread_mutex_lock(&trincoC);
        if(salaVIP_lugares >= TAM_vip)
        {
                if(numCliPrio > 0)
                {
                    sem_post(&semCliVIP);
                    numCliPrio--;
                }
                else
                {
                    if(numCliNorm > 0)
                    {
                        sem_post(&semCliNorm);
                        numCliNorm--;
                    }
                }
        }

        salaVIP_lugares--;
        pthread_mutex_unlock(&trincoC);
        
        pthread_mutex_lock(&trincoC);//TRINCO PARA EXPERIMENTAR
        prepara_mensg(SAIDA, clienteID, (int)(time(0) - start), salaID);
        pthread_mutex_unlock(&trincoC);
        
        printf("SAIU o cliente %d PRIO:%d da salaVIP\n", clienteID, cliente_prio);

    }
    
    return NULL;
}




int main()
{

    leitura();
    printf("O nº clientes a ser criado e': %d\n",nClientes);//NÃO USADO DE MOMENTO
    printf("A capacidade da sala e': %d\n",capacidadeSala);
    printf("A probablidade de desistencia da fila e': %d\n",probDesisteEspera);
    printf("A frequencia de criação de clientes por segundo e': %d\n", freqClientes);
    
    

    //cria socket stream
    if((sockfd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
        perror("erro ao criar socket cliente");
    
    bzero((char *) &end_serv, sizeof(end_serv));
    
    end_serv.sun_family = AF_UNIX;
    strcpy(end_serv.sun_path, UNIXSTR_PATH);
    dim_serv = strlen(end_serv.sun_path) + sizeof(end_serv.sun_family);
    
    if(connect(sockfd, (struct sockaddr *) &end_serv, dim_serv) < 0)
        perror("erro ao fazer connect no cliente");
    

    
    //Inicializar semáforos
    sem_init(&sala0, 0, capacidadeSala);// (semaforo, partilhado==0, valor)tamanho maximo da sala 0
    sem_init(&semCliVIP, 0, 0);
    sem_init(&semCliNorm, 0, 0);
    
    //varivel de tamanho da sala VIP
    TAM_vip = capacidadeSala;//tamanho maximo da sala vip
    
    start = time(0);
    printf("o tempo de iniciacao: %ld", start - start);
    
    srand(time(0));
    //preparaçao das tarefas
    pthread_t thread[nClientes];//colocar variavel "nClientes"
    
    for(int i = 0; i <nClientes; i++)
    {
        pthread_create(&thread[i], NULL, &cliente, NULL);
        usleep(50000);
        if(i%freqClientes == 0)//quantos clientes sao gerados de "uma só vez"
        {
            usleep(5000000);
        }
    }
    for(int j = 0; j < nClientes; j++)
    {
        pthread_join(thread[j], NULL);
        printf("Tarefa %d terminou\n",j);
    }
    
    usleep(5000);
    prepara_mensg(TERMINAR, 30, (int)(time(0) - start), 1);
    
    close(sockfd);
    exit(0);

        

    return 0;
}