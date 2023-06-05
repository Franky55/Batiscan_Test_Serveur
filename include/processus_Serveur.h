#ifndef PROCESSUSSERVEUR_H
#define PROCESSUSSERVEUR_H


typedef struct
{
    unsigned char CameraBuffer[5000];
    int Buffersize;

}PROCESSUS_SERVEUR;


int Processus_Serveur_initialise(void);

extern PROCESSUS_SERVEUR Processus_Serveur_Struct;
#endif