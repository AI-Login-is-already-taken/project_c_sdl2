#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#define MSGSIZE 100
#define CLIENTS_MAX 100

int main() {
    unsigned int clientlen;
    int nbClients=0;
    int sock;

    struct sockaddr_in *clientaddr[CLIENTS_MAX]; // Tableau des clients
    struct sockaddr_in client_adr; // Client courant
    char buf[MSGSIZE]; // Buffer avec taillé prédéfinie

    //Ouverture socket en mode UDP, datagramme, sur le domaine AF_INET
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //Configuration adresse du client
	client_adr.sin_family = AF_INET; // Connection inter-réseaux (UDP,TCP,etc)
	client_adr.sin_port = htons(2222); // Port de l'app
    client_adr.sin_addr.s_addr = INADDR_ANY; // Peut importe l'adresse recu !
    
    //Fourni un nom à la socket  
    bind(sock, (struct sockaddr*) &client_adr, sizeof(client_adr));

    // Boucle principale, infini.
    while (1) {
        clientlen = sizeof(struct sockaddr_in);
        printf("En attente d'un message...\n");
        // Allocation mémoire
        clientaddr[nbClients] = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        // Récupération du message dans le buffer.
        if(recvfrom(sock, buf, MSGSIZE, 0, (struct sockaddr *)clientaddr[nbClients], &clientlen)<0)
			perror("Error recvfrom : L41 - server.c");
        
        if (buf[0] == '@') // Si le serveur a recu le paquet de reconnaissance ...
			nbClients++; // ... on augmente le nombre de clients
		else
			for(int k = 0; k < nbClients; k++) // Pour chaques clients ...
				sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) clientaddr[k], clientlen); // ... on envoie un paquet contenant le buffer
				
		printf("Message(s) envoyé(s)...\n");
    }
    close (sock);
    
    return 0;
}
