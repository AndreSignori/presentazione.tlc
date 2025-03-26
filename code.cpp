// Inclusione delle librerie necessarie
#include <Arducam_Mega.h>  // Libreria per il controllo della fotocamera Arducam Mega
#include <SPI.h>           // Libreria per la comunicazione SPI (Serial Peripheral Interface)

// Definizione del pin Chip Select (CS) per la fotocamera
const int CS = 7;          // Pin digitale 7 usato per selezionare la fotocamera sul bus SPI
Arducam_Mega myCam(CS);    // Creazione dell'oggetto fotocamera con il pin CS specificato

void setup() {
  // Inizializzazione della comunicazione seriale a 9600 baud
  Serial.begin(9600);
  
  // Inizializzazione dell'interfaccia SPI hardware
  SPI.begin();
  
  // Inizializzazione della fotocamera
  myCam.begin();
  
  // Impostazione della risoluzione a 320x240 (QVGA) per limitare l'uso di RAM
  myCam.setResolution(ARDUCAM_MEGA_320x240);
}

void loop() {
  // Comando per acquisire un'immagine
  myCam.capture();
  
  // Attesa del completamento dell'acquisizione
  // ARDUCHIP_TRIG e CAP_DONE_MASK sono costanti della libreria che verificano lo stato
  while (!myCam.getBit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  
  // Lettura della dimensione dell'immagine acquisita
  uint32_t length = myCam.readFifoLength();
  
  // Allocazione dinamica di un buffer per contenere l'immagine
  uint8_t *buffer = (uint8_t *)malloc(length);
  
  // Lettura dell'immagine dal buffer della fotocamera
  myCam.readFifo(buffer, length);
  
  // Invio dell'immagine via seriale (al computer o altro dispositivo)
  Serial.write(buffer, length);
  
  // Liberazione della memoria allocata per il buffer
  free(buffer);
}
