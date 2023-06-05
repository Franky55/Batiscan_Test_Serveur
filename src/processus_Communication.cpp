#include "Arduino.h"
#include "main.h"

#include "interface_SPI_Master.h"
#include "serviceBaseDeTemps.h"
#include "service_Protocole_SPI.h"
#include <stdio.h>
#include "processus_Communication.h"
#include "processus_Serveur.h"


#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

// if udp:         https://gist.github.com/santolucito/70ecb94ce297eb1b8b8034f78683447b

void Processus_Communication_SPI();
void Processus_Communication_SPI_GERE_INFORECEIVED();
void Processus_Communication_Send_Image_Server();


PROCESSUS_COMMUNICATION processus_Communication_Struct;


int compteur_Com_SPI = 0;

/**
 * @brief La fonction initialise le processus de la communication
 * Met la LED bleu
 * -> processus_Communication_Set_New_Com
 * 
 * @return int 
 */
int Processus_Communication_initialise(void)
{
    
    serviceBaseDeTemps_execute[PROCESSUSCOMMUNICATION] = Processus_Communication_SPI;
    
    return 0;
}

void Processus_Communication_SPI()
{
    compteur_Com_SPI++;

    if(compteur_Com_SPI < 10)
    {
        return;
    }
    
    unsigned char data[255] = "";
    unsigned char out[255] = "";
    unsigned char tab_Size = 0;
    // Serial.println("SPI data sending");
    // Serial.print("Processus_Communication_SPI: processus_WIFI.tabReceived: ");

    // for(int i = 0; i < processus_WIFI.DataToSendSPI; i++)
    // {
    //     Serial.print((char)processus_WIFI.tabReceived[i]);
    // }
    // Serial.println("");
    //interface_SPI_MASTER_Transaction((unsigned char*)"dat", (unsigned char*)processus_WIFI.tabReceived, 3);
    service_Protocole_SPI_Pepare_Trame(data, &tab_Size);
    interface_SPI_MASTER_Transaction(data, out, tab_Size);
    interface_SPI_Master_Struct.Size = tab_Size;

    compteur_Com_SPI = 0;
    serviceBaseDeTemps_execute[PROCESSUSCOMMUNICATION] = Processus_Communication_SPI_GERE_INFORECEIVED;
}

void Processus_Communication_SPI_GERE_INFORECEIVED()
{
    // Serial.print("SPI slave sending:\n");

    // for(int i = 0; i < interface_SPI_Master_Struct.Size; i++)
    // {
    //     Serial.print(interface_SPI_Master_Struct.Received_SPI[i]);
    //     Serial.print(", ");
    // }
    // Serial.println("\n");
    service_Protocole_SPI_Received(interface_SPI_Master_Struct.Received_SPI, &interface_SPI_Master_Struct.Size);

    if(Service_Protocole_SPI_struct.state == STATE_NEW_IMAGE_READY)
    {
        serviceBaseDeTemps_execute[PROCESSUSCOMMUNICATION] = Processus_Communication_Send_Image_Server;
        Service_Protocole_SPI_struct.state = STATE_GET_NEW_IMAGE;
        return;
    }

    serviceBaseDeTemps_execute[PROCESSUSCOMMUNICATION] = Processus_Communication_SPI;
}


void Processus_Communication_Send_Image_Server()
{
    for(int i = 0; i < Service_Protocole_SPI_struct.Grosseur_Image; i++)
    {
        Processus_Serveur_Struct.CameraBuffer[i] = Service_Protocole_SPI_struct.New_ImageBuffer[i];
    }
    Processus_Serveur_Struct.Buffersize = Service_Protocole_SPI_struct.Grosseur_Image;
    Service_Protocole_SPI_struct.state = STATE_READING_IMAGE;
    serviceBaseDeTemps_execute[PROCESSUSCOMMUNICATION] = Processus_Communication_SPI;
}

