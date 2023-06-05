
#ifndef SERVICEPROTOCOLESPI_H
#define SERVICEPROTOCOLESPI_H

typedef struct
{
    uint8_t New_ImageBuffer[GROSSEUR_MAX_IMAGE];
    int PositionImage;
    int Grosseur_Image;
    unsigned char state;

}SERVICE_PROTOCOLE_SPI;


int service_Protocole_SPI_initialise(void);
int service_Protocole_SPI_Received(unsigned char* plane, unsigned char* size);
int service_Protocole_SPI_Pepare_Trame(unsigned char* plane, unsigned char* size);


extern SERVICE_PROTOCOLE_SPI Service_Protocole_SPI_struct;
#endif