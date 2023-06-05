
#ifndef INTERFACESPIMASTER_H
#define INTERFACESPIMASTER_H

typedef struct 
{
    unsigned char Received_SPI[255];
    unsigned char Transmit_SPI[255];
    unsigned char Size;
}INSTERFACE_SPI_Master;



int interface_SPI_MASTER_initialise();
int interface_SPI_MASTER_Transaction(unsigned char * data, unsigned char * out, unsigned long size);


extern INSTERFACE_SPI_Master interface_SPI_Master_Struct;

#endif