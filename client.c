/* cliTCPIt.c - Exemplu de client TCP
   Trimite un nume la server; primeste de la server "Hello nume".
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char msg[100],aux[100];		// mesajul trimis
  int sursa;
  
  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  printf("Dati fisierul in care veti rezolva problema: ");
  fflush(stdout);
  if(read(0,&aux,99)<0)
  {
      perror("[client]Eroare la read de la tastatura\n");
      return errno;
  }
  aux[strlen(aux)-1]=0;
  if((sursa=open(aux,O_RDONLY))<0)
  {
      perror ("[client]Eroare la open fisier.\n");
      return errno;
  }
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }
  bzero(msg,100);
  if (read (sd, &msg, 100) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
    
    char s[5]={0};
    int sec,i=0;
    while(msg[i]!=':')
    {
        s[i]=msg[i];
        i++;
    }
    sec=atoi(s);
    
    //printf("%s\n",msg+i+1);
    msg[strlen(msg)]='\n';
    msg[strlen(msg)+1]=0;
    write(1,msg+i+1,100);
    printf("Timp de rezolvare: %d secunde\n",sec);
    fflush(stdout);
  
  sleep(sec);
  
  while(read(sursa,&msg,100)>0)
  {
  if (write (sd, msg, strlen(msg)) < 0)
    {
      perror ("[client]Eroare la write() de la server.\n");
      return errno;
    }
    bzero(msg,100);
  }
  /* afisam mesajul primit */
  bzero(msg,strlen(msg));
  if (read (sd, &msg, 100) <= 0)
    {
      perror ("[client]Eroare la read() spre server.\n");
      return errno;
    }
  printf("%s\n",msg);
  /* inchidem conexiunea, am terminat */
  close (sd);
}
