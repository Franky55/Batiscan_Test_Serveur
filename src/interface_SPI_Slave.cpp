#include <Arduino.h>
#include "main.h"
#include "interface_SPI_Slave.h"
#include "serviceBaseDeTemps.h"
#include <ESP32SPISlave.h>
//#include <ESP32DMASPISlave.h>

#define INTERFACEGPIO10_COMPTE  (\
  GPIO10_TEMPS_POUR_RESET_EN_MS * SERVICEBASEDETEMPS_FREQUENCE_EN_HZ \
  /1000.0)
#define INTERFACE_RESET_MASTER 100


void interface_SPI_Start_SPI();
void interface_SPI_Queue_Transaction();
void interface_SPI_Data_Available();
void interface_SPI_ReadData();


ESP32SPISlave slave;
INTERFACESPISTRUCT interface_SPI_Struct;

unsigned char RAW_RX_buf[SPI_BUFFER_SIZE] {0};
unsigned long interface_Compteur_Master_Connecte = 0;

int spi_compt_available = 0;

/**
 * @brief L'initialisation de l'interface SPI
 * reset des variables a zero
 * -> interface_SPI_Reset_Master
 * 
 * @return int 
 */
int interface_SPI_SLAVE_initialise()
{
    pinMode(INTERFACE_SPI_CS1, OUTPUT);
    interface_Compteur_Master_Connecte = 0;
    interface_SPI_Struct.etatDuModule = 0;
    interface_SPI_Struct.trameReady = 0;
    //le processus s'occupe de la grosseur du message
    memset(interface_SPI_Struct.spi_slave_rx_buf, 0, SPI_BUFFER_SIZE);
    memset(interface_SPI_Struct.spi_slave_tx_buf, 0, SPI_BUFFER_SIZE);
    
    serviceBaseDeTemps_executeDansLoop[INTERFACESPI_TRANSACTION] = interface_SPI_Start_SPI;  //va reset le master
    
    return 0;
}



/**
 * @brief Initialisation de la communication SPI
 * -> interface_SPI_Queue_Transaction
 * 
 */
void interface_SPI_Start_SPI()
{
  
    //digitalWrite(GPIO10, HIGH);
    pinMode(INTERFACE_SPI_CS1, INPUT);
    pinMode(SPI_CLK, INPUT);
    pinMode(SPI_MOSI, INPUT);

    //interface_NEOPIXEL_allume(0, 100, 0);

    interface_Compteur_Master_Connecte = 0;
    slave.setDataMode(SPI_MODE0);
    slave.begin(SPI2_HOST, SPI_CLK, SPI_MISO, SPI_MOSI, INTERFACE_SPI_CS1);
    slave.setQueueSize(1);
    serviceBaseDeTemps_executeDansLoop[INTERFACESPI_TRANSACTION] = interface_SPI_Queue_Transaction;
}


/**
 * @brief Tant qu'une transaction n'est pas pret j'attend
 * Le processusCommunication va mettre mettre trameReady a 1 lorsqu'il est pret
 * -> interface_SPI_Data_Available
 */
void interface_SPI_Queue_Transaction()
{
    //interface_NEOPIXEL_allume(0, 10, 0);
    if(interface_SPI_Struct.trameReady == 0)
    {
        return;
    }

    // Serial.print("SPI slave sending:\n");

    // for(int i = 0; i < interface_SPI_Struct.spi_message_size; i++)
    // {
    //     Serial.print(interface_SPI_Struct.spi_slave_tx_buf[i]);
    //     Serial.print(", ");
    // }
    // Serial.println("\n");


    
    slave.end();
    slave.setDataMode(SPI_MODE0);
    slave.begin(SPI2_HOST, SPI_CLK, SPI_MISO, SPI_MOSI, INTERFACE_SPI_CS1);
    slave.setQueueSize(1);
    interface_SPI_Struct.trameReady = 0;

    slave.queue(RAW_RX_buf, interface_SPI_Struct.spi_slave_tx_buf, interface_SPI_Struct.spi_message_size);
    

    serviceBaseDeTemps_executeDansLoop[INTERFACESPI_TRANSACTION] = interface_SPI_Data_Available;
 
}

/**
 * @brief Cette fonction check si du data est available
 * S'il y a du data j'appelle la fonction interface_SPI_ReadData()
 * 
 */
void interface_SPI_Data_Available()
{
    if(slave.available())
    {
        interface_SPI_ReadData();
    }
}


/**
 * @brief Cette fonction permet de lire le data de la communication SPI
 * Le data va se mettre dans le tableau interface_SPI_Struct.spi_slave_rx_buf
 * interface_SPI_Struct.spi_message_size = grosseur du message
 * interface_SPI_Struct.etatDuModule = 1
 * -> interface_SPI_Queue_Transaction
 * 
 */
void interface_SPI_ReadData()
{
    
    interface_SPI_Struct.spi_message_size = (unsigned char)slave.size();

    
    // Serial.print("Data Received ");
    // Serial.print(interface_SPI_Struct.spi_message_size);
    // Serial.print(" : ");

    for(int i = 0; i < interface_SPI_Struct.spi_message_size; i++)
    {
        // Serial.print((char)RAW_RX_buf[i]);
        interface_SPI_Struct.spi_slave_rx_buf[i] = RAW_RX_buf[i];
    }
    // Serial.println("");
    memset(RAW_RX_buf, 0, SPI_BUFFER_SIZE);

    // you should pop the received packet
    slave.pop();

    while(slave.available())
    {
        slave.pop();
    }


    interface_SPI_Struct.etatDuModule = 1;
    
    serviceBaseDeTemps_executeDansLoop[INTERFACESPI_TRANSACTION] = interface_SPI_Queue_Transaction;
}

