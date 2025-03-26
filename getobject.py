
//pip install opencv-python 

import cv2
import numpy as np
import requests

# URL del server ESP32-CAM
esp32_url = "http://<ESP32_CAM_IP>/"

# Carica il classificatore Haar per il riconoscimento facciale (o un altro classificatore)
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

def get_image_from_esp32():
    # Richiesta HTTP per ottenere l'immagine JPEG
    response = requests.get(esp32_url, stream=True)
    img_array = np.asarray(bytearray(response.content), dtype=np.uint8)
    img = cv2.imdecode(img_array, -1)
    return img

def detect_faces(img):
    # Converte l'immagine in scala di grigi per il riconoscimento facciale
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    # Rileva i volti
    faces = face_cascade.detectMultiScale(gray, 1.1, 4)
    
    # Disegna un rettangolo attorno ai volti rilevati
    for (x, y, w, h) in faces:
        cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
    
    return img

def main():
    while True:
        # Ottieni l'immagine dalla ESP32-CAM
        img = get_image_from_esp32()
        
        # Rileva i volti nell'immagine
        img_with_faces = detect_faces(img)
        
        # Mostra l'immagine con i volti rilevati
        cv2.imshow("ESP32-CAM Image", img_with_faces)
        
        # Esci se si preme il tasto 'q'
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main() 
