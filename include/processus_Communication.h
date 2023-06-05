#ifndef PROCESSUSCOMMUNICATION_H
#define PROCESSUSCOMMUNICATION_H




typedef struct
{
    uint8_t CameraBuffer[GROSSEUR_MAX_IMAGE];

}PROCESSUS_COMMUNICATION;



int Processus_Communication_initialise(void);

extern PROCESSUS_COMMUNICATION processus_Communication_Struct;
#endif