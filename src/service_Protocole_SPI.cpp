
#include "Arduino.h"
#include "main.h"
#include "service_Protocole_SPI.h"
#include "Processus_Communication.h"

#include <stdio.h>

#define TRAME_SIZE 64

#define NOMBRE_DATA_PAR_CHUNK 60

#define START_BYTE 'M'
#define START_BYTE_NEW_IMAGE 'I'
#define START_BYTE_RECEIVE 'C'

#define POSITION_START_BYTE 0
#define POSITION_START_NEW_CHUNK_HAUT 1
#define POSITION_START_NEW_CHUNK_BAS 2
#define POSITION_DATA 3
#define POSITION_CHECKSUM 63


SERVICE_PROTOCOLE_SPI Service_Protocole_SPI_struct;


int service_Protocole_SPI_Read_Data(unsigned char* plane, unsigned char* size);

/**
 * @brief Return 0;
 * 
 * @return int 
 */
int service_Protocole_SPI_initialise(void)
{
    Service_Protocole_SPI_struct.PositionImage = 0;
    Service_Protocole_SPI_struct.Grosseur_Image = 0;
    Service_Protocole_SPI_struct.state = STATE_GET_NEW_IMAGE;
    return 0;
}


/**
 * @brief La fonction check le packet recu 
 * 1er check la grosseur: return -3
 * 2e check START_BYTE: return -1
 * 3e check le checkSum: return -2
 * 
 * All good return 0
 * 
 * @param plane 
 * @param size 
 * @return int 
 */
int service_Protocole_SPI_Received(unsigned char* plane, unsigned char* size)
{
    if((*size) < TRAME_SIZE)            //La trame n'est pas asse long
    {
        return -3;
    }

    if(plane[0] != START_BYTE_RECEIVE)  //Pas de start byte
    {
        return -1;
    }

    unsigned char CheckSum = 0;
    for(int i = 0; i < (*size) - 1; i++)
    {
        CheckSum = CheckSum + plane[i];
    }

    if(CheckSum != plane[(*size)-1])    //Pas bon check sum
    {
        return -2;
    }

    return service_Protocole_SPI_Read_Data(plane, size); //update les valeurs
}


/**
 * @brief Met les variable dans la structure des valeurs WANTED
 * 
 * @param plane 
 * @param size 
 * @return int 
 */
int service_Protocole_SPI_Read_Data(unsigned char* plane, unsigned char* size)
{
    *size = TRAME_SIZE;
    

    Service_Protocole_SPI_struct.PositionImage = (plane[POSITION_START_NEW_CHUNK_HAUT] << 8) + plane[POSITION_START_NEW_CHUNK_BAS];
    
    
    for(int i = POSITION_DATA; i < POSITION_CHECKSUM; i++)
    {        
        if(Service_Protocole_SPI_struct.PositionImage + i - POSITION_DATA >= Service_Protocole_SPI_struct.Grosseur_Image)
        {
            Service_Protocole_SPI_struct.state = STATE_NEW_IMAGE_READY;
            break;
        }
        Service_Protocole_SPI_struct.New_ImageBuffer[Service_Protocole_SPI_struct.PositionImage + i - POSITION_DATA] = plane[i];
        
    }



    // Serial.println("");

    return 0;
}

/**
 * @brief Prepare une trame a envoyer au master
 * Tous les etats des accesoires sont mis dans la trame
 * 
 * @param plane 
 * @param size 
 * @return int 
 */
int service_Protocole_SPI_Pepare_Trame(unsigned char* plane, unsigned char* size)
{
    
    *size = TRAME_SIZE;
    

    plane[POSITION_START_BYTE] = START_BYTE;
    
    
    for(int i = 1; i < POSITION_CHECKSUM; i++)
    {        
        plane[i] = 0;
    }

    unsigned char checkSum = 0;
    for (int i = 0; i < POSITION_CHECKSUM; i++)
    {
        checkSum += plane[i];
    }

    plane[POSITION_CHECKSUM] = checkSum;
    return 0;
}



int service_Protocole_SPI_Pepare_Trame_New_Image(unsigned char* plane, unsigned char* size)
{
    *size = TRAME_SIZE;
    

    plane[POSITION_START_BYTE] = START_BYTE_NEW_IMAGE;
    plane[1] = START_BYTE;
    plane[2] = (Service_Protocole_SPI_struct.Grosseur_Image >> 8) & 0x00FF;
    plane[3] = (Service_Protocole_SPI_struct.Grosseur_Image) & 0x00FF;
    
    for(int i = 4; i < POSITION_CHECKSUM; i++)
    {
        plane[i] = 0;
    }

    unsigned char checkSum = 0;
    for (int i = 0; i < POSITION_CHECKSUM; i++)
    {
        checkSum += plane[i];
    }

    plane[POSITION_CHECKSUM] = checkSum;
    
    
    return 0;
}
