//#############################################################################
/**
 * @file main.c
 * @author Francis Gratton
 * @brief
 * Program file containing the code defined in xmain.h
 * Please refer to this other file for information necessary in order to make this work.
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
//#############################################################################


//INCLUSIONS

#include "main.h"

// inlcude des pilotes




// Include des Services
#include "serviceTaskServer.h"
#include "serviceBaseDeTemps.h"
#include "service_Protocole_SPI.h"

// Inlude des interfaces
#include "interface_SPI_Slave.h"
#include "interface_SPI_Master.h"

// Include des processus
#include "processus_Communication.h"
#include "processus_Serveur.h"

//Definitions privees
//pas de definitions privees
//#include <Adafruit_NeoPixel.h>




/// @brief Fonction qui fait l'initialisation de tout les modules permettant
//   au fonctionnement global du véhicule.
/// @param void
void main_initialise(void);

//Definitions de variables privees:
//pas de variables privees

//Definitions de fonctions privees:

void main_initialise(void)
{

  serviceTaskServer_initialise();
  serviceBaseDeTemps_initialise();

  service_Protocole_SPI_initialise();

  interface_SPI_MASTER_initialise();

  //interfaceEntree1_initialise();
  Processus_Communication_initialise();
  Processus_Serveur_initialise();
}

void setup(void) 
{
  Serial.begin(115200);
  main_initialise();
  

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  // END of Trinket-specific code.

  

  serviceTaskServer_DemarreLesTachesALaTouteFinDeSetup();
}

void loop(void) 
{
  serviceTaskServer_gestion.execute();
  serviceBaseDeTemps_gereDansLoop();   
}

//Definitions de variables publiques:
//pas de variables publiques

//Definitions de fonctions publiques:
//pas de fonctions publiques
