/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
void construire_message(char *message, char motif, int lg) {
	int i;
	for (i=0;i<lg;i++){
	message[i] = motif;
	}
}
void afficher_message(char *message, int lg) {
	int i;
	printf("message construit : ");
	for (i=0;i<lg;i++) {
		printf("%c", message[i]); 
		printf("\n");
	}
}


void main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int mode = -1 ; /* 0=UDP, 1=TCP */
	int sock ;
	int port ;
	struct hostent * hp ;
	struct sockaddr_in adr_local ;
	struct sockaddr_in adr_distant ;
	struct sockaddr_in adr_em ;
	int lg_mesg = 40 ;
	int lg_max = 100 ;
	char * pmesg = malloc(lg_mesg) ;
	int lg_adr_em=sizeof(adr_em) ;
	int lg_adr_local=sizeof(adr_local) ;
	int lg_adr_dist=sizeof(adr_distant) ;

	while ((c = getopt(argc, argv, "pn:su")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		case 'u':
			mode = 0;
			break;

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}

    port = atoi(argv[argc-1]);
	port = htons(port);

	if (mode == 0){
		if (source == 1){
			printf("On est dans la source\n");
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0))==-1){
				printf("échec de création du socket\n");
				exit(1);
			}

			memset((char*)&adr_distant, 0, lg_adr_dist); /*reset socket distant*/
			adr_distant.sin_family=AF_INET;
			adr_distant.sin_port=port;

			if ((hp = gethostbyname(argv[argc-2]))==NULL){
				printf("erreur gethostbyname\n");
				exit(1);
			}
			memcpy((char*)&(adr_distant.sin_addr.s_addr),
					hp->h_addr,
					hp->h_length);

			nb_message=10;
			printf("Le nombre de tampon à envoyer est %d.\n",nb_message);
			for (int i=0;i<nb_message;i++){
				construire_message(pmesg, (char)((i%26)+97), lg_mesg);
				afficher_message(pmesg,lg_mesg);
				sendto(sock,pmesg,lg_mesg,0,(struct sockaddr *)&adr_distant,lg_adr_dist);
			}

			if(close(sock)==-1){
			printf("échec de destruction du socket\n");
			exit(1);
			}
		}

		if (source == 0){
			printf("On est dans le puits\n");
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0))==-1){
				printf("échec de création du socket\n");
				exit(1);
			}
			memset((char*)&adr_local, 0, lg_adr_local); /*reset socket local*/
			adr_distant.sin_family=AF_INET;
			adr_distant.sin_port=port;
			adr_local.sin_addr.s_addr = INADDR_ANY ;

			if(bind(sock, (struct sockaddr *)&adr_local, lg_adr_local)==-1){
				printf("échec du bind\n");
				exit(1);
			}

			while ((lg_mesg=recvfrom(sock, pmesg, lg_max,0,(struct sockaddr *)&adr_em, &lg_adr_em)) != -1){
				afficher_message(pmesg,lg_mesg);
			}

			if(close(sock)==-1){
			printf("échec de destruction du socket\n");
			exit(1);
			}
		}

	}

	if (source == 1)
		printf("on est dans le source\n");
	else
		printf("on est dans le puits\n");

	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}
}

