#include <WiFi.h>               // Libreria per la connessione WiFi (ESP32)
#include <WebServer.h>          // Libreria per la gestione del server web
#include <Arducam_Mega.h>       // Libreria per la gestione della fotocamera Arducam Mega
#include <SPI.h>                // Libreria per la comunicazione SPI

// Credenziali della rete WiFi a cui connettersi
const char* ssid = "TUA_WIFI";         // Nome della rete WiFi
const char* password = "TUA_PASSWORD"; // Password della rete WiFi

// Password per accedere alla foto tramite il server web
const char* accessPassword = "mypass"; 

// Definizione del pin CS (Chip Select) per la fotocamera Arducam Mega
const int CS = 7;

// Creazione dell'oggetto fotocamera Arducam utilizzando il pin CS
Arducam_Mega myCam(CS);

// Creazione del server web sulla porta 80
WebServer server(80);

void setup() {
  Serial.begin(115200); // Inizializzazione della comunicazione seriale a 115200 baud

  SPI.begin();          // Inizializzazione del bus SPI
  myCam.begin();        // Inizializzazione della fotocamera
  myCam.setResolution(ARDUCAM_MEGA_320x240); // Impostazione della risoluzione dell'immagine a 320x240

  // Connessione alla rete WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connessione WiFi in corso...");
  
  // Attendere finché il modulo non si connette al WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Stampa l'indirizzo IP assegnato al modulo ESP
  Serial.println("\nConnesso al WiFi!");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());

  // Configurazione del server web: gestione della richiesta GET a "/foto"
  server.on("/foto", HTTP_GET, []() {
    // Verifica se è stata fornita la password corretta nell'URL
    if (!server.hasArg("pass") || server.arg("pass") != accessPassword) {
      server.send(401, "text/plain", "Accesso negato! Usa ?pass=mypass"); // Messaggio di errore in caso di password errata
      return;
    }
    sendPhoto(); // Se la password è corretta, invia la foto
  });

  server.begin(); // Avvia il server web
}

void loop() {
  server.handleClient(); // Gestisce le richieste dei client connessi
}

// Funzione per scattare una foto e inviarla al client web
void sendPhoto() {
  myCam.capture(); // Acquisizione dell'immagine

  // Attendere fino a quando la cattura dell'immagine non è completata
  while (!myCam.getBit(ARDUCHIP_TRIG, CAP_DONE_MASK));

  // Lettura della dimensione dell'immagine acquisita
  uint32_t length = myCam.readFifoLength();

  // Allocazione dinamica di un buffer per contenere l'immagine
  uint8_t *buffer = (uint8_t *)malloc(length);

  // Lettura dei dati dell'immagine dal buffer della fotocamera
  myCam.readFifo(buffer, length);

  // Impostazione degli header HTTP per il browser (indicando il tipo di contenuto immagine)
  server.sendHeader("Content-Type", "image/jpeg");
  server.sendHeader("Content-Length", String(length));

  // Invio dell'immagine come risposta alla richiesta HTTP
  server.send(200, "image/jpeg", (const char*)buffer, length);

  // Liberazione della memoria allocata per il buffer
  free(buffer);
}
