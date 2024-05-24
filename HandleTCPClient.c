#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include "Garden.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

void HandleTCPClient(int clntSocket, Garden *gardenptr)
{
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
    
    int workPlace[2] = {-1, -1};

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    if (echoBuffer[0] == '1') {
        if (gardenptr->first_pos[1] + gardenptr->first_way < 0 ||
            gardenptr->first_pos[1] + gardenptr->first_way == gardenptr->n) {
            ++gardenptr->first_pos[0];
            gardenptr->first_way *= -1;
            if (gardenptr->first_pos[0] == gardenptr->m) {
                echoBuffer[0] = '\0';
                recvMsgSize = 1;
            }
        } else {
            gardenptr->first_pos[1] += gardenptr->first_way;
        }
        workPlace[0] = gardenptr->first_pos[0];
        workPlace[1] = gardenptr->first_pos[1];
    }

    if (echoBuffer[0] == '2') {
        if (gardenptr->second_pos[0] + gardenptr->second_way < 0 ||
            gardenptr->second_pos[0] + gardenptr->second_way == gardenptr->m) {
            --gardenptr->second_pos[0];
            gardenptr->second_way *= -1;
            if (gardenptr->second_pos[0] == gardenptr->-1) {
                echoBuffer[0] = '\0';
                recvMsgSize = 1;
            }
        } else {
            gardenptr->second_pos[1] += gardenptr->second_way;
        }
        workPlace[0] = gardenptr->second_pos[0];
        workPlace[1] = gardenptr->second_pos[1];
    }

    if (echoBuffer[0] == '3') {
        printf("Garden is:\n");
        for (size_t i = 0; i < gardenptr->m; ++i) {
            for (size_t j = 0; j < gardenptr->n; ++j) {
                printf("%d\t", gardenptr->field[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        sleep(1);
    }

    if (workPlace[0] != -1) {
        sleep(1);
        if (gardenptr->field[workPlace[0]][workPlace[1]] == 0) {
            sleep(atoi(&echoBuffer[2]));
            gardenptr->field[workPlace[0]][workPlace[1]] = echoBuffer[0] - '0';
        }
        sprintf(echoBuffer, "Worker %c work in place (%d, %d)", echoBuffer[0], workPlace[0], workPlace[1]);
        printf("Worker %c work in place (%d, %d)", echoBuffer[0], workPlace[0], workPlace[1]);
    }

    /* Send received string and receive again until end of transmission */
    if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
        DieWithError("send() failed");

    close(clntSocket);    /* Close client socket */
}

