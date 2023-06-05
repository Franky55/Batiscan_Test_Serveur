
#ifndef INTERFACESPISLAVE_H
#define INTERFACESPISLAVE_H

/// @brief Structure public qui permet de savoir et de changer l'état du module  
typedef struct
{
    unsigned char etatDuModule; 
    unsigned char trameReady;
    unsigned char spi_message_size; 
    unsigned char spi_slave_tx_buf[SPI_BUFFER_SIZE];
    unsigned char spi_slave_rx_buf[SPI_BUFFER_SIZE];
} INTERFACESPISTRUCT;

//Fonctions publiques:
void interfaceEntree1_initialise(void);


int interface_SPI_SLAVE_initialise();
int interface_SPI_SLAVE_Transaction(const unsigned char * data, unsigned char * out, unsigned long size);

//Variables publiques:
extern INTERFACESPISTRUCT interface_SPI_Struct;

#endif