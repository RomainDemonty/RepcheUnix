#include "FichierClient.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int estPresent(const char* nom)
{
  // TO DO
  int pos,fd,i=1,N;
  CLIENT client;

  if((fd = open(FICHIER_CLIENTS,O_RDONLY))==-1)
  {
    pos = -1;
  }
  else
  {
    pos=0;
    N = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);
    N =  N/sizeof(CLIENT);
    while(i<=N)
    {
      read(fd,&client,sizeof(client));
      if(strcmp(nom,client.nom)==0)
      {
        pos=i;
      }
      i++;
    }
    if(close(fd))
    {
      pos = -1;
    }
  }
  return pos;
}

////////////////////////////////////////////////////////////////////////////////////
int hash(const char* motDePasse)
{
  // TO DO
  int hash, N, i;

  N = strlen(motDePasse);
  i = 0;
  hash = 0;

  while(i<N)
  {
    hash = hash + (i+1)*motDePasse[i];
    i++;
  }

  hash = hash %97;
  return hash;
}

/////////////////////////////////////////
//////////////////////////////////////////
void ajouteClient(const char* nom, const char* motDePasse)
{
  // TO DO
  int fd,mdp;

    if((fd = open(FICHIER_CLIENTS,O_WRONLY | O_CREAT | O_APPEND,0644))==-1)
    {
        printf("Mauvaise ouverture du fichier");
    }
    else
    {
      mdp=(hash(motDePasse));
      lseek(fd,0,SEEK_END);
      write(fd,nom,20);
      write(fd,&mdp,sizeof(int));
      if(close(fd))
      {
        printf("Mauvaise fermeture du fichier");
      }
    }  
}

////////////////////////////////////////////////////////////////////////////////////
int verifieMotDePasse(int pos, const char* motDePasse)
{
  // TO DO
  int fd, V, h, h2;
  if((fd = open(FICHIER_CLIENTS,O_RDONLY))==-1)
  {
    V = -1;
  }
  else
  {
    h = hash(motDePasse);
    lseek(fd,(pos-1)*sizeof(CLIENT),SEEK_SET);
    lseek(fd,20,SEEK_CUR);
    read(fd,&h2,sizeof(int));
    printf("La vérification a donné:\nMot de passe encodé : %d\n Mot de passe BD : %d \n",h,h2);
    if(h==h2)
    {
      V = 1;
    }
    else
    {
      V=0;
    }
    if(close(fd))
    {
      V = -1;
    }
  }
  return V;
}

////////////////////////////////////////////////////////////////////////////////////
int listeClients(CLIENT *vecteur) // le vecteur doit etre suffisamment grand
{
  // TO DO
  int i=0 , fd,N;
  CLIENT client;

    if((fd = open(FICHIER_CLIENTS,O_RDONLY))==-1)
    {
        printf("Mauvaise ouverture du fichier");
    }
    else
    {
      N = lseek(fd,0,SEEK_END);
      lseek(fd,0,SEEK_SET);
      N =  N/sizeof(CLIENT);
      while(i < N)
      {
        read(fd,&client,sizeof(CLIENT));
        strcpy(vecteur->nom,client.nom);
        vecteur->hash = client.hash;
        i++;
        vecteur++;
      }
      if(close(fd))
      {
        printf("Mauvaise fermeture du fichier");
      }
    }


  return i;
}
