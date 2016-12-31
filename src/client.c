#include <stdio.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "display.h"
#include <pthread.h>
#include <stdlib.h>

int sock;
struct sockaddr_in server_adr;

/*
 * Thread qui permet de gerer l'affichage de la fenetre
 */
void *task_checkEvents (void *p_data)
{
	while (!checkEvents());	
	displayQuit();	
	pthread_exit(NULL);
}

/*
 * Thread qui permet de gerer les paquets recu.
 */
void *task_receiveCommands (void *p_data)
{
	while ( 1 ) {
		char buffer[300];
		unsigned int taille = sizeof(server_adr); // unsigned : nombre positif seulement
		if(recvfrom(sock, buffer, sizeof(buffer)-1, 0, (struct sockaddr*) &server_adr, &taille)<0)
			perror("Error recvfrom : L28 - client.c");
	
		char valueLetter[2];		
		memcpy(valueLetter, &buffer[0], 1); // Copie le premier byte de buffer dans valueLetter
		valueLetter[1] = '\0'; //On termine par le caractère de fin de chaine
		
		// Initilisation des différentes variables utilisées
		char lettre[20], alpha[20], red[20], green[20], blue[20];		
		char charx1[20], chary1[20], charx2[20], chary2[20];		
		char width[30], hight[30];		
		char image[30];		
		int a,r,v,b;			
		int x1,x2,y1,y2;		
		int x,y,l,h;
				
		// Switch de la lettre pour différencier la commande recu.
		switch (valueLetter[0]) {
			case 'R':
				sscanf(buffer, "%s %s %s %s %s %s %s %s %s", lettre, charx1, chary1, width, hight, alpha, red, green, blue);
				a = atoi(alpha);
				r = atoi(red);
				v = atoi(green);
				b = atoi(blue);
				x = atoi(charx1);
				y = atoi(chary1);
				l = atoi(width);
				h = atoi(hight);
				displayDrawRect(x, y, l, h, a, r, v, b, true);
				displayPersistentScreen();
				break;
			case 'C':
				sscanf(buffer, "%s %s %s %s %s", lettre, alpha, red, green, blue);
				a = atoi(alpha);
				r = atoi(red);
				v = atoi(green);
				b = atoi(blue);
				displayDrawRect(0, 0, 3000, 3000, a, r, v, b, true);
				displayPersistentScreen();
				break;
			case 'L': 
				sscanf(buffer, "%s %s %s %s %s %s %s %s %s", lettre, charx1, chary1, charx2, chary2, alpha, red, green, blue);
				a = atoi(alpha);
				r = atoi(red);
				v = atoi(green);
				b = atoi(blue);
				x1 = atoi(charx1);
				x2 = atoi(charx2);
				y1 = atoi(chary1);
				y2 = atoi(chary2);
				displayDrawLine(x1, y1, x2, y2, a, r, v, b);
				displayPersistentScreen();
				break;
			case 'Q':
				displayQuit();
				exit(0);
				break;
		}
	}
	pthread_exit(NULL);
}

/*
 * Fonction qui affiche l'aide
 */
void help() {
	printf("Commande :\n");
	printf("   - C <A> <R> <G> <B> pour effacer la fenètre.\n");
	printf("   - L <X1> <Y1> <X2> <Y2> <A> <R> <G> <B> pour tracer une ligne.\n");
	printf("   - R <X> <Y> <width> <hight> <A> <R> <G> <B> pour tracer un rectangle.\n");
	printf("   - H pour voir les commandes.\n");
	printf("   - Q pour quitter.\n");
}

void main(){	
    //Ouverture socket en mode UDP, datagramme, sur le domaine AF_INET
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //Configuration adresse du server
	server_adr.sin_family = AF_INET; // Connection inter-réseaux (UDP,TCP,etc)
	server_adr.sin_port = htons(2222); // Port de l'app
	server_adr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse de l'app
	
    //Initialisation de la zone de dessin
	displayInit("ProjetRéseauC",600,800);	
	
    //Création des threads pour permettre de gerer plusieurs fonctons en même temps.
	pthread_t threadCheckEvents, threadCommands;
    pthread_create (&threadCheckEvents, NULL, task_checkEvents, NULL); // Gère l'affichage de la fenetre
    pthread_create (&threadCommands, NULL, task_receiveCommands, NULL); // Recoie les commandes du serveur
    
    //Envoi du paquet de 'reconnaissance', qui permet au server de savoir si un nouveau client est connecté.
    if(sendto(sock, "@", 30, 0, (struct sockaddr*) &server_adr, sizeof(server_adr))<0)
        perror("Error sendto : L112 - client.c");
    
    int exit = 0;
    // Boucle principale, tant qu'il n'a pas quitté
	while(!exit) { 
		char cmd[30];
		printf("Entrez une commande (H pour voir les commandes):\n");
		// Récupération de la commande tappé
		// Regex : Longueur max de 30-1 (Eviter overflow). Avec forcement un caratère de fin de ligne a la fin de la commande.
		scanf("%29[^\r\n]", cmd);
		while(getchar()!='\n'){}; // Vidage du buffer
		
		// Si il n'a choisi de quitter
		if(cmd[0] != 'Q') { 
			// Si la personne a indiqué H en commande ou si c'est sa première connexion
			if (cmd[0] == 'H' || cmd[0] == '@') { 
				help();
			} else {				
				//Envoi du paquet avec la commande, vers le serveur.
				if(sendto(sock, cmd, 30, 0, (struct sockaddr*) &server_adr, sizeof(server_adr))<0)
					perror("Error sendto : L127 - client.c");
				else
					printf("Commande envoyée avec succés...\n");
			}
		} else {
			exit = 1;
		}
	}	
	close(sock);
}
