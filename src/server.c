#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#include <netdb.h>

#define BUFSIZE 100
#define MAX_CLIENTS 100

int main() {
    printf("#########################\n");
    printf("#########SERVEUR#########\n");
    printf("#########################\n");

    unsigned int clientlen;
    int i=0;
    int length = 0;
    int n;
    int sockfd;

    struct sockaddr_in *clientaddr[MAX_CLIENTS]; /* adresse client */
    struct sockaddr_in adr;
    char buf[BUFSIZE]; /* message du buffer */

    /*
     * socket: créer un socket parent
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd<0) perror("socket");

    /*
     * créer l'adresse internet du serveur
     */
    adr.sin_family = AF_INET;
    adr.sin_port = htons(1234);
    adr.sin_addr.s_addr = INADDR_ANY;

    /*
     * bind: associe les sockets parents au port défini
     */
    if (bind(sockfd, (struct sockaddr*) &adr, sizeof(adr))<0)
        perror("bind");

    while (1)
    {
        clientlen = sizeof(struct sockaddr_in);
        printf("En attente d'un message...\n");
        /*
         * recvfrom: reçoit un datagrame de type UDP d'un client
         */
        clientaddr[i] = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)clientaddr[i], &clientlen);

        sleep(2);
        printf("Envoi du message ne cours...\n");
        /*
         * sendto: réecrit à tout les clients ce qu'il a reçu
         */
        length = i + 1;
        for(int k = 0; k < length; k++) {
            n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) clientaddr[k], clientlen);
            if (n < 0)
                perror("error sendto");
            else
                printf("Message n°%d envoyé...\n",k);
        }
        i++;
    }

    //Fermeture de la socket
    close (sockfd);

    return 0;
}
