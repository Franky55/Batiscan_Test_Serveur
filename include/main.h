/**
 * @file xmain.h
 * @author Francis Gratton 
 * 
 * @brief Le fichier xmain.h est nécessaire pour les includions global qui
 *   contien les defines essentiel au fonctionnement. Le processeur utilisé
 *  pour la gestion du véhicule est un ESP32-S3. Voir le document de projet
 *  pour plus d'information sur le fonctionnement.
 *      
 *  Le programme suivant fonctionne avec une base de temps et des machine d'état.
 * 
 *   
 * @version 0.0.1
 * @date 2023-05-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MAIN_H
#define MAIN_H

//PROGRAMME: Batiscan
//DESCRIPTION: programme qui va permettre le bon fonctionnement d'un sous-marin

//HISTORIQUE:
/* Historique du programme gèrer par git et publier sur github au lien suivant
*
*       https://github.com/Franky55/Template
*/
  
//  *** INFORMATION PUBLIQUE ***


//DEFINITIONS REQUISES PAR LE MODULE:
//Dependances materielles:
#define GROSSEUR_MAX_IMAGE 5000


#define STATE_NEW_IMAGE_READY 1
#define STATE_READING_IMAGE 2
#define STATE_GET_NEW_IMAGE 3


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***  DÉFINITION POUR LA COMMUNICATION SPI  ***/
//#define MISO 7
//#define MOSI 8
//#define CLK 6
//#define CS1 9

#define SPI_MISO 13
#define SPI_MOSI 11
#define SPI_CLK 12
#define INTERFACE_SPI_CS1 2
#define INTERFACE_SPI_CS2 14

#define SPI_BUFFER_SIZE 255


/***  DÉFINITION POUR LES SERVOS MOTEURS  ***/

//Les pins utilisés
#define SERVO1 9



//Pin utilisé pour la communication I2C
#define I2C_SDA 7
#define I2C_SCL 8


//Utlisation de la LED Neopixel
#define PIN_NEOPIXEL  48
#define NUMPIXELS 1 


/***  DÉFINITION NÉCESSAIRE À LA BASE DE TEMPS  ***/
/**
 * @brief Période en microsecondes à laquelle le TaskServer exécute sa fonction 
 *  qui fait apelle au service Base de temps pour gêrer plusieur tache a la fois
 * 
 */
#define SERVICETASKSERVER_PERIODE_EN_US (1000L)
/**
 * @brief Fréquence en Hz de la base de temps
 * 
 */
#define SERVICEBASEDETEMPS_FREQUENCE_EN_HZ 1000
/**
 * @brief Nombre de phase dans une période de la base de temps (1 si pas utilisé)
 */
#define SERVICEBASEDETEMPS_NOMBRE_DE_PHASES_DANS_LOOP 2

/**
 * @brief Phase pour la communication SPI
 * 
 */
#define INTERFACESPI_TRANSACTION 1



/**
 * @brief Nombre de phase de la base de temps \ref xserviceBaseDeTemps.h
 * 
 */
#define SERVICEBASEDETEMPS_NOMBRE_DE_PHASES 1




/**
 * @brief Phase pour la communication
 * 
 */
#define PROCESSUSCOMMUNICATION 0



#define PROCESSUSCLIGNOTANT_TEMPS_ALLUME_EN_MS 5
#define PROCESSUSCLIGNOTANT_TEMPS_ETEINT_EN_MS 500


#endif