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
#include "readPNG.h"

int s;
struct sockaddr_in adr;

void *task_a (void *p_data)
{
	while ( !checkEvents() );
	
	displayQuit();	
	pthread_exit(NULL);
}

void *task_b (void *p_data)
{
	while ( 1 ) {
		char buffer[300];
		unsigned int taille = sizeof(adr);
		int len = recvfrom(s, buffer, sizeof(buffer)-1, 0, (struct sockaddr*) &adr, &taille);
		
		//printf("Buff: %s \n",buffer);
		char subbuff[2];
		memcpy( subbuff, &buffer[0], 1 );
		subbuff[1] = '\0';
		
		char lettre[20];
		
		char alpha[20];
		char rouge[20];
		char vert[20];
		char bleu[20];
		
		char charx1[20];
		char chary1[20];
		char charx2[20];
		char chary2[20];
		
		char largeur[30];
		char hauteur[30];
		
		char image[30];
		
		int a;
		int r;
		int v;
		int b;		
		
		switch (subbuff[0]) {
			case 'C':
				sscanf(buffer, "%s %s %s %s %s", lettre, alpha, rouge, vert, bleu);
				a = atoi(alpha);
				r = atoi(rouge);
				v = atoi(vert);
				b = atoi(bleu);
				//printf("Alpha: %d R: %d V: %d B: %d\n", a, r, v, b);
				displayDrawRect(0, 0, 3000, 3000, a, r, v, b, true);
				displayPersistentScreen();
				break;
			case 'L': 
				sscanf(buffer, "%s %s %s %s %s %s %s %s %s", lettre, charx1, chary1, charx2, chary2, alpha, rouge, vert, bleu);
				a = atoi(alpha);
				r = atoi(rouge);
				v = atoi(vert);
				b = atoi(bleu);
				int x1 = atoi(charx1);
				int x2 = atoi(charx2);
				int y1 = atoi(chary1);
				int y2 = atoi(chary2);
				//printf("Alpha: %d R: %d V: %d B: %d X1: %d X2: %d Y1: %d Y2: %d\n", a, r, v, b, x1, x2, y1, y2);
				displayDrawLine(x1, y1, x2, y2, a, r, v, b);
				displayPersistentScreen();
				break;
			case 'R':
				sscanf(buffer, "%s %s %s %s %s %s %s %s %s", lettre, charx1, chary1, largeur, hauteur, alpha, rouge, vert, bleu);
				a = atoi(alpha);
				r = atoi(rouge);
				v = atoi(vert);
				b = atoi(bleu);
				int x = atoi(charx1);
				int y = atoi(chary1);
				int l = atoi(largeur);
				int h = atoi(hauteur);
				//printf("Alpha: %d R: %d V: %d B: %d X: %d Y: %d Largeur: %d Hauteur: %d\n", a, r, v, b, x, y, l, h);
				displayDrawRect(x, y, l, h, a, r, v, b, true);
				displayPersistentScreen();
				break;
			case 'I':
				sscanf(buffer, "%s %s %s %s", lettre, image, charx1, chary1);
				int xi = atoi(charx1);
				int yi = atoi(chary1);
				
				int width, height;
				void *pixels;
				pngReadFile(image, &width, &height, &pixels);
				displayPixels(pixels,xi,yi,width,height,1, false);
				
				break;
			case 'Q':
				displayQuit();
				exit(0);
				break;
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  
	printf("#########################\n");
	printf("#########CLIENT##########\n");
	printf("#########################\n");

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s<0) perror("socket");

	adr.sin_family = AF_INET;
	adr.sin_port = htons(1234);
	adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	displayInit("test",800,600);	
	
	pthread_t ta;
    pthread_create (&ta, NULL, task_a, NULL);
    
    pthread_t tb;
    pthread_create (&tb, NULL, task_b, NULL);
    
    sendto(s, "@", 30, 0, (struct sockaddr*) &adr, sizeof(adr));
    
	int quit = 0;
	while(!quit) 
	{
		//Envoyez un message				
		char cmd[30];
		printf("Commande a envoyer (q pour quitter):\n");
		scanf("%29[^\r\n]", cmd);
		while(getchar()!='\n'){}; //vider buffer	
		
		if ( cmd[0] == 'H' ) {
			printf("Commande :\n");
			printf("   - C <A> <R> <G> <B> pour effacer la fenètre.\n");
			printf("   - L <X1> <Y1> <X2> <Y2> <A> <R> <G> <B> pour tracer une ligne.\n");
			printf("   - R <X> <Y> <Largeur> <Hauteur> <A> <R> <G> <B> pour tracer un rectangle.\n");
			printf("   - I <image.png> <X> <Y> pour dessiner une image.\n");
			printf("   - H pour voir les commandes.\n");
			printf("   - Q pour quitter.\n");
		} else {
			sendto(s, cmd, 30, 0, (struct sockaddr*) &adr, sizeof(adr));
			sleep(2);
			printf("Commande envoyé !\n");
		}
	}
	//Fermeture de la socket
	close (s);

	return 0;
}
