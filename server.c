/* servTCPIt.c - Exemplu de server TCP iterativ
   Asteapta un nume de la clienti; intoarce clientului sirul
   "Hello nume".
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

/* portul folosit */
#define PORT 2025

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int compilare()
{
    int teava[2];
    if(pipe(teava)<0)
    {
        perror("[server.c]Eroare la pipe\n");
        exit(0);
    }
    switch(fork())
    {
        case -1:
        {
            perror("[server.c]Eroare la fork\n");
            exit(0);
        }
        case 0:
        {
            if(close(teava[0])<0)
            {
                perror("[server.c]Eroare la close teava\n");
                exit(0);
            }
            if(dup2(teava[1],2)<0)
            {
                perror("[server.c]Eroare la dup2\n");
                exit(0);
            }
            char pid[10],nume[10];
            sprintf(pid,"%d.c",getppid());
            sprintf(nume,"%d.exe",getppid());
            if(creat(nume,S_IRWXU|S_IRWXG|S_IRWXO)<0)
            {
                perror("[server.c]Eroare la creat executabil");
                exit(0);
            }
            execlp("gcc","gcc",pid,"-o",nume,0);
            perror("[server.c]Eroare la execle\n");
            exit(0);
        }
        default:
        {
            if(close(teava[1])<0)
            {
                perror("[server.c]Eroare la close teava\n");
                exit(0);
            }
            wait(0);
            char buf[200];
            if(read(teava[0],&buf,200)<0)
            {
                perror("[server.c]Eroare la read\n");
                exit(0);
            }
            if(strstr(buf,"error"))
                return -1;
            return 0;
        }
    }
}

int evaluare(int probl)
{
    char fisi[20];
    int punctaj=0,aux,iesire;
    for(int i=1;i<=5;i++)
    {
        int aux;
        
        sprintf(fisi,"aux_%d",getpid());
        if((aux=creat(fisi,S_IRWXU|S_IRWXG|S_IRWXO))<0)
        {
            perror("[server.c]Eroare la creare test\n");
            exit(0);
        }
        
        switch(fork())
        {
            case -1:
            {
                perror("[cerver.c]Eroare la fork\n");
                exit(0);
            }
            case 0:
            {
                int intrare;
                char nume[10]={0};
                sprintf(nume,"%d.exe",getppid());
                sprintf(fisi,"%d_%d",probl,i);
                if((intrare=open(fisi,O_RDONLY))<0)
                {
                    perror("Eraore la open intrare");
                    return errno;
                }
                
                if(dup2(intrare,0)<0)
                {
                    perror("Eroare la dup2!\n");
                    return errno;
                }
                if(dup2(aux,1)<0)
                {
                    perror("Eroare la dup2!\n");
                    return errno;
                }
                
                execl(nume,nume);
                perror("Eroare la execlp!\n");
                exit(0);
            }
            default:
            {
                wait(0);
                char corect[100]={0},real[100]={0};
                
                sprintf(fisi,"%d_%d_i",probl,i);
                if((iesire=open(fisi,O_RDONLY))<0)
                {
                 perror("[server.c]Eroare la deschidere test\n");
                 exit(0);
                }
                
                int auxx;
                sprintf(fisi,"aux_%d",getpid());
                if((auxx=open(fisi,O_RDONLY))<0)
                {
                    perror("eroare la open fisi\n");
                }
                
                if(read(auxx,&real,100)<0)
                {
                    perror("Eroare la read!!!!!!!!!!!!\n");
                    exit(0);
                }
                if(read(iesire,&corect,100)<0)
                {
                    perror("Eroare la read!\n");
                    exit(0);
                }
                
                close(auxx),close(iesire);
                
                corect[strlen(corect)-1]=0;
                
                if(strcmp(corect,real)==0)
                    punctaj+=20;
            }
        }
    }
    close(aux);
    return punctaj;
}

int main ()
{
  struct sockaddr_in server;
  struct sockaddr_in from;
  int sd,clienti[100],conf,asteapta,probleme[100],nr_prob,lista;
  char buf[20]={0},*p;char buffy[100];
  
  if((lista=open("rezultate",O_RDWR|O_TRUNC|O_CREAT))<0)
  {
      perror("[server]Eroare la crearea fisier rezultate\n");
      return errno;
  }
  
  system("chmod 777 rezultate");
  
  if((conf=open("config.cfg",O_RDONLY))<0)
  {
      perror("[server]Eroare la open config!\n");
      return errno;
  }
  
  if(read(conf,&buf,20)<0)
  {
      perror("[server]Eroare la read config!\n");
      return errno;
  }
  
  p=strtok(buf,"\n");
  asteapta=atoi(p);
  p=strtok(0,"\n");
  nr_prob=atoi(p);
  
  for(int i=0;i<nr_prob;i++)
  {
      sprintf(buf,"pb%d",i);
      if((probleme[i]=open(buf,O_RDONLY))<0)
      {
          perror("[server]Eroare la open prbleme!");
          return errno;
      }
  }
  
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }

  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (PORT);
  
  int op=1;
  if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&op,sizeof(op))==-1)
  {
      perror("[server]Eroare la setsockopt!\n");
      return errno;
  }
  
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
  {
      perror ("[server]Eroare la bind().\n");
      return errno;
  }

  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  int length = sizeof (from);

  for(int ii=0;ii<asteapta;ii++)
  {
    
    if((clienti[ii] = accept (sd, (struct sockaddr *) &from, &length))<0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
	
	int problema=random()%nr_prob;
    switch(fork())
     {
         case -1:
         {
             perror("[server.c]Eroare la fork!\n");
             close(clienti[ii]);
         }
         case 0: 
         {
         if(close (sd)<0)
         {
             perror("[server.c]Eroare la cloase!\n");
             return errno;
         }
         
         char msgrasp[1000],msg[2024];
         int sursa;
         
         sprintf(msgrasp,"%d.c",getpid());
         if((sursa=creat(msgrasp,777))<0)
         {
             perror("[server.c]Eroare la open sursa\n");
             close (clienti[ii]);
             exit(0);
         }
         
         bzero(msgrasp,100);
         
         if(lseek(probleme[problema], 0, SEEK_SET)<0)
         {
             perror("[server.c]Eroare la lseek!");
             close (clienti[ii]);
             exit(0);
         }
         
         if((read(probleme[problema],&msgrasp,1000))<0)
         {
             perror("[server.c]Eroare la read prbolema!\n");
             close (clienti[ii]);
             exit(0);
         }
         sprintf(msgrasp+strlen(msgrasp),"Id-ul dumneavoastra este:%d",getpid());
         msgrasp[strlen(msgrasp)]=0;
         
         if (write(clienti[ii], msgrasp, strlen(msgrasp)) <= 0)
	     {
	       perror ("[server.c]Eroare la write() catre client.\n");
	       close (clienti[ii]);
	       exit(0);	/* continuam sa ascultam */
	     }
	     else
	     {
	     printf ("[server.c]Mesajul a fost trasmis cu succes.\n");
	     fflush(stdout);
         }
         
         bzero(msg,100);
         while((op=read(clienti[ii], &msg, 2024)) > 0)
	     {
	       if(op<0)
	       {
	         perror ("[server.c]Eroare la read() de la client.\n");
	         close (clienti[ii]);
	         exit(0);
	       }
	       if(write(sursa,msg,op)<0)
	       {
	           perror("[server.c]Eroare la write in fisierul sursa\n");
	           close (clienti[ii]);
	           exit(0);
	       }
	       if(op<=2048)break;
	     }
	     
	     if(lseek(lista, 0, SEEK_END)<0)
            {
             perror("[server.c]Eroare la lseek!");
             close (clienti[ii]);
             exit(0);
            }
	     
	     if(compilare()==-1)
	     {
	        sprintf(msgrasp,"%d are 0 puncte\n",getpid());
	        
	        if(write(lista,msgrasp,strlen(msgrasp))<0)
	        {
	           perror("[server.c]Eroare la write in fisierul rezultat\n");
	           close (clienti[ii]);
	           exit(0);
	        }
	     }
	     else
	     {
	         op=evaluare(problema);
	         sprintf(msgrasp,"%d are %d puncte\n",getpid(),op);
	         
	         if(write(lista,msgrasp,strlen(msgrasp))<0)
	         {
	           perror("[server.c]Eroare la write in fisierul rezultat\n");
	           close (clienti[ii]);
	           exit(0);
	         }
	     }
         
         
         sprintf(msg,"%d.c",getpid());
         if(remove(msg)<0)
         {
             perror("[server.c]Eroare la stergere fisiere auuxiliare!\n");
             return errno;
         }
         sprintf(msg,"%d.exe",getpid());
         if(remove(msg)<0)
         {
             perror("[server.c]Eroare la stergere fisiere auuxiliare!\n");
             return errno;
         }
         sprintf(msg,"aux_%d",getpid());
         if(remove(msg)<0)
         {
             perror("[server.c]Eroare la stergere fisiere auuxiliare!\n");
             return errno;
         }
         exit(0);
        }
     }
    }
    
    for(int ii=1;ii<=asteapta;ii++)
        wait(0);
    
    if(lseek(lista,0,SEEK_SET)<0)
    {
        perror("[server]Eroare la lseek\n");
        exit(0);
    }
    
    if(read(lista,&buffy,99)<0)
    {
        perror("[server]Eroare la read din liste\n");
        exit(0);
    }
    
    for(int ii=0;ii<asteapta;ii++)
    {
        if(write(clienti[ii],buffy,strlen(buffy))<0)
        {
          perror("[server]Eroare la ultim write!\n");
          return errno;
        }
      if(close(clienti[ii])<0)
        perror("[server]Eroare la ultim close!\n");
    }
    remove("rezultate");
    return 0;
}
