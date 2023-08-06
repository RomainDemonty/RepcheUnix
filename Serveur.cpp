#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "protocole.h" // contient la cle et la structure d'un message

#include "FichierClient.h"

int idQ,idShm,idSem;
int fdPipe[2];
TAB_CONNEXIONS *tab;

void afficheTab();

void handlerSIGINT(int sig);

int main()
{
  // Armement des signaux
  // TO DO

  //Si on veut couper le serveur et la file de message en même temps
  struct sigaction A;
  A.sa_handler =handlerSIGINT;
  A.sa_flags = 0;
  sigaction(SIGINT,&A, NULL);

  // Creation des ressources
  // Creation de la file de message
  fprintf(stderr,"(SERVEUR %d) Creation de la file de messages\n",getpid());
  if ((idQ = msgget(CLE,IPC_CREAT | IPC_EXCL | 0600)) == -1)  // CLE definie dans protocole.h
  {
    perror("(SERVEUR) Erreur de msgget");
    exit(1);
  }

  // TO BE CONTINUED

  // Creation du pipe
  // TO DO

  // Initialisation du tableau de connexions
  tab = (TAB_CONNEXIONS*) malloc(sizeof(TAB_CONNEXIONS)); 

  for (int i=0 ; i<6 ; i++)
  {
    tab->connexions[i].pidFenetre = 0;
    strcpy(tab->connexions[i].nom,"");
    tab->connexions[i].pidCaddie = 0;
  }
  tab->pidServeur = getpid();
  tab->pidPublicite = 0;

  afficheTab();

  // Creation du processus Publicite (étape 2)
  // TO DO

  // Creation du processus AccesBD (étape 4)
  // TO DO

  MESSAGE m;
  MESSAGE reponse;

  while(1)
  {
  	fprintf(stderr,"(SERVEUR %d) Attente d'une requete...\n",getpid());
    if (msgrcv(idQ,&m,sizeof(MESSAGE)-sizeof(long),1,0) == -1)
    {
      perror("(SERVEUR) Erreur de msgrcv");
      msgctl(idQ,IPC_RMID,NULL);
      exit(1);
    }

    switch(m.requete)
    {
      case CONNECT :  // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CONNECT reçue de %d\n",getpid(),m.expediteur);
                      for(int i = 0 ; i < 6 ; i++)
                      {
                        if(tab->connexions[i].pidFenetre == 0)
                        {
                          tab->connexions[i].pidFenetre = m.expediteur;
                          i = 6 ;
                        }
                      }
                      break;

      case DECONNECT : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete DECONNECT reçue de %d\n",getpid(),m.expediteur);
                      for(int i = 0 ; i < 6 ; i++)
                      {
                        if(tab->connexions[i].pidFenetre == m.expediteur)
                        {
                          tab->connexions[i].pidFenetre = 0;
                          i = 6 ;
                        }
                      }
                      break;
      case LOGIN :    // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete LOGIN reçue de %d : --%d--%s--%s--\n",getpid(),m.expediteur,m.data1,m.data2,m.data3);
                      if(m.data1 == 1)
                      {
                        if(estPresent(m.data2) > 0)
                        {
                          strcpy(m.data4,"<<Client déjà existant !>>");
                          m.data1 = 0 ;
                        }
                        else
                        {
                          strcpy(m.data4,"<< Nouveau client bien créé : Bienvenue !>>");
                          for(int i = 0 ; i < 6 ; i++)
                          {
                            if(tab -> connexions[i].pidFenetre == m.expediteur)
                            {
                              strcpy(tab->connexions[i].nom,m.data2);
                              i = 6;
                            }
                            ajouteClient(m.data2,m.data3);
                            m.data1 = 1;
                          }
                        }
                      }
                      else
                      {
                        if(estPresent(m.data2) > 0)
                        {
                          if(verifieMotDePasse(estPresent(m.data2), m.data3) == 1)
                          {
                            strcpy(m.data4,"<<Re-bonjour cher Client !>>");
                            for(int i = 0 ; i < 6 ; i++)
                            {
                              if(tab -> connexions[i].pidFenetre == m.expediteur)
                              {
                                strcpy(tab->connexions[i].nom,m.data2);
                                i = 6;
                              }
                              m.data1 = 1;
                            }
                          }                        
                          else
                          {
                            strcpy(m.data4,"<<Mot de passe incorect...>>");
                            m.data1 = 0;
                          }
                        }
                        else
                        {
                          strcpy(m.data4 ,"<<Client Inconnu>>");
                          m.data1 = 0;
                        }
                      }

                      m.type = m.expediteur;
                      if(msgsnd(idQ, &m , sizeof(MESSAGE) - sizeof(long) , 0) == -1)
                      {
                        perror("Erreur d'envoi");
                        msgctl(idQ , IPC_RMID, NULL);
                        exit(1);
                      }


                      //Permet de renvoyer a l'expéditeur le message qui correspond au résultat de la connection
                      kill(m.type,SIGUSR1);
                      break; 

      case LOGOUT :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete LOGOUT reçue de %d\n",getpid(),m.expediteur);
                      for(int i = 0 ; i < 6 ; i++)
                      {
                        if(tab->connexions[i].pidFenetre == m.expediteur)
                        {
                          strcpy(tab->connexions[i].nom,"");
                          i = 6 ;
                        }
                      }
                      break;

      case UPDATE_PUB :  // TO DO
                      break;

      case CONSULT :  // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);
                      break;

      case ACHAT :    // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);
                      break;

      case CADDIE :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CADDIE reçue de %d\n",getpid(),m.expediteur);
                      break;

      case CANCEL :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);
                      break;

      case CANCEL_ALL : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CANCEL_ALL reçue de %d\n",getpid(),m.expediteur);
                      break;

      case PAYER : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete PAYER reçue de %d\n",getpid(),m.expediteur);
                      break;

      case NEW_PUB :  // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete NEW_PUB reçue de %d\n",getpid(),m.expediteur);
                      break;
    }
    afficheTab();
  }
}

void afficheTab()
{
  fprintf(stderr,"Pid Serveur   : %d\n",tab->pidServeur);
  fprintf(stderr,"Pid Publicite : %d\n",tab->pidPublicite);
  fprintf(stderr,"Pid AccesBD   : %d\n",tab->pidAccesBD);
  for (int i=0 ; i<6 ; i++)
    fprintf(stderr,"%6d -%20s- %6d\n",tab->connexions[i].pidFenetre,
                                                      tab->connexions[i].nom,
                                                      tab->connexions[i].pidCaddie);
  fprintf(stderr,"\n");
}


//Action des signaux :

void handlerSIGINT(int sig)
{
  printf("Fin du serveur et de la file de message \n");
  msgctl(idQ,IPC_RMID,NULL);
  exit(1);
}