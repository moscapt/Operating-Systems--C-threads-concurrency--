#include "util.h"

int pararsimulacao = 0;
int sockfd, novosockfd, dim_cli, dim_serv, pid_filho;
struct sockaddr_un end_cli, end_serv;

//variaveis de controlo
int clienteTotal, desistenciasTotais, tempoTotalSimulacao;
int contadorTempoEntradas, tempoMedioEntrada, contadorDesistencias, contadorEntradas;
int arrayCliChegadas[200];
int arrayCliTempoEntradas[200];
int contadorSala0, contadorSala1;
int contadorChegadas;
void escrita()
{
    FILE* logs;

    logs = fopen("Logs.txt", "a");

    if(logs == NULL)
    {
        printf("ERRO AO CRIAR FICHEIRO");
    }
    else
    {
        fprintf(logs, "\n---SIMULAÇÃO INICIADA---\n");
        

        //int a = 0;
        while(1)
        {
            char buffer[5];
            read(novosockfd, buffer, sizeof(buffer));
            
            //printf("%s\n", buffer);
            
            if(buffer[0] == 'A')//criacao de cliente
            {
                if(buffer[2] < 0)
                {
                    printf("Chegou um cliente a fila seu numero é: %d, chegou aos: %d minutos!\n", buffer[1], (256 + buffer[2]));
                    arrayCliChegadas[contadorChegadas] = buffer[2];
                    contadorChegadas++; 
                    fprintf(logs, "Chegou um cliente a fila seu numero é: %d, chegou aos: %d minutos!\n", buffer[1], (256 + buffer[2]));
                    clienteTotal++;
                }
                else
                {
                    printf("Chegou um cliente a fila seu numero é: %d, chegou aos: %d minutos!\n", buffer[1], buffer[2]);
                    arrayCliChegadas[contadorChegadas] = buffer[2];
                    contadorChegadas++;
                    fprintf(logs, "Chegou um cliente a fila seu numero é: %d, chegou aos: %d minutos!\n", buffer[1], buffer[2]);
                    clienteTotal++;
                }
            }
            if(buffer[0] == 'B')//entrada de cliente na disco
            {
                if(buffer[2] < 0)
                {
                    printf("\nEntrou na discoteca o cliente: %d, na sala %d, entrou apos %d minutos na fila de espera\n",buffer[1], buffer[3], (256 + buffer[2]));
                    arrayCliTempoEntradas[contadorTempoEntradas] = (256 + buffer[2]);
                    contadorTempoEntradas++;
                    if(buffer[3] == 0)
                    {
                        contadorSala0++;
                    }//contador da sala
                    else
                    {
                        contadorSala1++;
                    }
                    fprintf(logs, "\nEntrou na discoteca o cliente: %d, entrou apos %d minutos na fila de espera\n", buffer[1], (256 + buffer[2]));
                    contadorEntradas++;
                }
                else
                {
                    printf("\nEntrou na discoteca o cliente: %d,na sala %d entrou apos %d minutos na fila de espera\n", buffer[1],buffer[3],  buffer[2]);
                    arrayCliTempoEntradas[contadorTempoEntradas] = buffer[2];
                    contadorTempoEntradas++;
                    if(buffer[3] == 0)
                    {
                        contadorSala0++;
                    }//contador da sala
                    else
                    {
                        contadorSala1++;
                    }//contador da sala
                    fprintf(logs, "\nEntrou na discoteca o cliente: %d, entrou apos %d minutos na fila de espera\n", buffer[1], buffer[2]);
                    contadorEntradas++;
                }
            }
            if(buffer[0] == 'C')//desistencia de um cliente
            {
                if(buffer[2] < 0)
                {
                    printf("\nDesistiu da discoteca o cliente: %d, entrou apos %d minutos na fila de espera\n", buffer[1], (256 + buffer[2]));
                    fprintf(logs, "\nDesistiu da discoteca o cliente: %d, entrou apos %d minutos na fila de espera\n", buffer[1], (256 + buffer[2]));
                    contadorDesistencias++;
                }
                else
                {
                    printf("\nDesistiu da discoteca o cliente: %d, entrou apos %d minutos na fila de espera\n", buffer[1], buffer[2]);
                    fprintf(logs, "\nDesistiu da discoteca o cliente: %d, entrou apos %d minutos na fila de espera\n", buffer[1], buffer[2]);
                    contadorDesistencias++;
                }
            }
            if(buffer[0] == 'D')//saida da disco
            {
                if(buffer[2] < 0)
                {
                    printf("\nSaiu da discoteca o cliente: %d, sala %d no minuto:%d \n", buffer[1], buffer[3], (256 + buffer[2]));
                    fprintf(logs, "\nSaiu da discoteca o cliente: %d, no minuto:%d \n", buffer[1], (256 + buffer[2]));
                }
                else
                {
                    printf("\nSaiu da discoteca o cliente: %d, sala %d no minuto:%d \n", buffer[1], buffer[3],  buffer[2]);
                    fprintf(logs, "\nSaiu da discoteca o cliente: %d, no minuto:%d \n", buffer[1], buffer[2]);
                }
            }
            if(buffer[0] == 'F')
            {   
                if(buffer[2] < 0)
                {
                    printf("\nFoi Encerrado a discoteca, apos %d minutos!\n", (256 + buffer[2]));
                    tempoTotalSimulacao = (256 + buffer[2]);
                    break; //este break estara numa letra que significara que foi terminado a simulacao
                }
                else
                {
                    printf("\nFoi Encerrado a discoteca, apos %d minutos!\n",  + buffer[2]);
                    tempoTotalSimulacao = buffer[2];
                    break; //este break estara numa letra que significara que foi terminado a simulacao
                }
            }
            
        }
        fprintf(logs, "---SIMULAÇÃO TERMINADA---\n\n");

    }

    fclose(logs);
}

void estatisticas()
{
    FILE* logs;

    logs = fopen("Logs.txt", "a");

    if(logs == NULL)
    {
        printf("ERRO AO CRIAR FICHEIRO");
    }
    else
    {
        fprintf(logs, "\n\nINICIO DAS ESTATISTICAS\n");
        printf("\n\nINICIO DAS ESTATISTICAS\n\n");
        fprintf(logs, "Foram gerados %d clientes ao todo\n", clienteTotal);
        printf("Foram gerados %d clientes ao todo\n", clienteTotal);
        for(int i = 0; i < contadorTempoEntradas; i++)
        {
            tempoMedioEntrada = tempoMedioEntrada + arrayCliTempoEntradas[i]; //formula esta errada, voltar a rever
        }
        tempoMedioEntrada = tempoMedioEntrada/clienteTotal;
        fprintf(logs, "A media de espera para entrar dos clientes foi %d min!\n", tempoMedioEntrada);
        printf("A media de espera para entrar dos clientes foi %d min!\n", tempoMedioEntrada);
        fprintf(logs, "Entraram na sala0 %d e na salaVIP %d\n", contadorSala0, contadorSala1);
        printf("Entraram na sala0 %d e na salaVIP %d\n", contadorSala0, contadorSala1);
        fprintf(logs, "Número de desistencias %d!\n", contadorDesistencias);
        printf("Número de desistencias %d!\n", contadorDesistencias);
        fprintf(logs, "Número de clientes que entrou %d, foram gerados %d€ em bilhetes de entrada\n", contadorEntradas, (contadorSala0*5 + contadorSala1*10));
        printf("Número de clientes que entrou %d, foram gerados %d€ em bilhetes de entrada\n", contadorEntradas, (contadorSala0*5 + contadorSala1*10));
        fprintf(logs, "Duração total da simulação foi de %d minutos!\n", tempoTotalSimulacao);
        printf("Duração total da simulação foi de %d minutos!\n", tempoTotalSimulacao);
        fprintf(logs, "\n\nFIM DAS ESTATISTICAS\n");
        printf("\nFIM DAS ESTATISTICAS\n");
    }
}

void trata_cliente_stream(int sockfd)
{
    int n =0;
    char buffer[MAXLINHA+1];
    
    n = read(sockfd, buffer, MAXLINHA+1);
    if(n < 0)
    {
        perror("erro servidor no read");
    }
    
    printf("%s", buffer);
    printf("asdfsa");
    
    if(write(sockfd, buffer,n) != n)
        perror("erro servidor no write");

}

int main()
{
        

    if((sockfd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
            perror("erro ao criar socket stream servidor");
    }
    
    unlink(UNIXSTR_PATH);
    
    bzero((char *)&end_serv, sizeof(end_serv));
    end_serv.sun_family =AF_UNIX;
    strcpy(end_serv.sun_path, UNIXSTR_PATH);
    dim_serv = strlen(end_serv.sun_path) + sizeof(end_serv.sun_family);
    if(bind(sockfd, (struct sockaddr *) &end_serv, dim_serv) <0)
    {
            perror("erro ao atribuir nome socket servidor");
    }
    
    listen(sockfd, 1);
    
    for(;;)
    {
        dim_cli = sizeof(end_cli);
                
        novosockfd = accept(sockfd, (struct sockaddr *) &end_cli, &dim_cli);
        if(novosockfd < 0)
        {
            perror("erro ao criar ligacao dedicada: accept");
        }
            
        if((pid_filho = fork()) < 0)
        {
            perror("erro ao criar processo para atender cliente");
        }
        else if(pid_filho == 0)
        {
            close(sockfd);
            exit(0);
        }
        printf("INICIO DA SIMULACAO\n\n");
        escrita();
        estatisticas();

        
        close(novosockfd);
        printf("FIM DA SIMULACAO\n");
        break;
    }
    

    printf("Ficheiro Logs.txt criado com exito!\n");

    return 0;

}
