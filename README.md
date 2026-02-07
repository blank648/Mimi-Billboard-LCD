# 📋 DOCUMENTAȚIE TEHNICĂ - MINI BILLBOARD LED

## 1. DESCRIEREA PROIECTULUI

Mini Billboard este un panou LED programabil format din două matrice WS2812 8×8, controlat de o placă ESP32 Plusivo . Sistemul permite afișarea de mesaje text derulante, configurabile prin interfață web, accesibilă prin WiFi. La fiecare actualizare a mesajului, un buzzer activ emite o confirmare sonoră.

## 2. COMPONENTE HARDWARE

### 2.1 Lista Componentelor

| Componentă | Specificații | Cantitate | Rol |
| :-- | :-- | :-- | :-- |
| ESP32 Plusivo | Microcontroler cu WiFi | 1 | Unitate centrală de procesare și server web |
| Matrice LED WS2812 | 8×8 pixeli RGB | 2 | Display vizual pentru mesaje |
| Logic Level Converter | CJMCU-0108 (8 canale) | 1 | Conversie semnal 3.3V → 5V |
| Buzzer activ | 5V, 2 pini | 1 | Feedback sonor |
| Sursă de alimentare | HW131, 5V | 1 | Alimentare matrice LED |

### 2.2 Schema de Conectare

#### ESP32 → Logic Level Converter:

- **GPIO 13** (ESP32) → **LV1** (Low Voltage input, canal 1)
- **GND** (ESP32) → **GND LV side**
- **3.3V** (ESP32) → **LV** (Low Voltage power)


#### Logic Level Converter → Matrice LED:

- **HV1** (High Voltage output, canal 1) → **DIN** (prima matrice)
- **5V** (sursa HW131) → **HV** (High Voltage power)
- **GND** (sursa HW131) → **GND HV side**


#### Matrice LED:

- **DOUT** (prima matrice) → **DIN** (a doua matrice)
- **5V** (sursa HW131) → **5V** (ambele matrice)
- **GND** (sursa HW131) → **GND** (ambele matrice)


#### ESP32 → Buzzer:

- **GPIO 14** (ESP32) → **Pin +** (buzzer)
- **GND** (ESP32) → **Pin -** (buzzer)


#### Alimentare:

- **Sursa HW131 5V** → Matrice LED + Logic Level Converter (HV side)
- **ESP32** → Alimentat prin USB sau Vin (5V poate fi conectat și la sursa HW131 dacă este necesar)


## 3. ARHITECTURĂ SOFTWARE

### 3.1 Biblioteci Utilizate

```cpp
WiFi.h           // Conectivitate WiFi
WebServer.h      // Server HTTP
Adafruit_GFX.h   // Grafică de bază
Adafruit_NeoMatrix.h  // Control matrice LED
Adafruit_NeoPixel.h   // Control LED-uri WS2812
```


### 3.2 Configurare Hardware (în cod)

| Parametru | Valoare | Descriere |
| :-- | :-- | :-- |
| `PIN_LED` | 13 | GPIO conectat la DIN prin level shifter |
| `PIN_BUZZER` | 14 | GPIO pentru buzzer |
| `MAT_W` | 8 | Lățime matrice individuală |
| `MAT_H` | 8 | Înălțime matrice individuală |
| `TILES_X` | 2 | Număr matrice pe orizontală |
| `TILES_Y` | 1 | Număr matrice pe verticală |
| Luminozitate | 20 | Valoare 0-255 |
| Culoare text | Roșu | RGB(255, 0, 0) |

### 3.3 Funcționalități Principale

#### A. Server Web

- **Port**: 80 (HTTP)
- **Endpoint principal** (`/`): Interfață HTML pentru introducere mesaj
- **Endpoint configurare** (`/set`): Primește mesajul via parametru GET `msg`


#### B. Afișare Text

- **Mod**: Scroll orizontal, dreapta → stânga
- **Viteză**: Refresh la 80ms (12.5 FPS)
- **Font**: 5×7 pixeli (lățime caracter ≈ 6 pixeli)
- **Wrap**: Dezactivat pentru scroll continuu


#### C. Feedback Buzzer

Secvență la primirea mesajului nou:

1. HIGH (100ms)
2. LOW (50ms)
3. HIGH (100ms)
4. LOW

## 4. INSTALARE ȘI CONFIGURARE

### 4.1 Pregătirea Mediului de Dezvoltare

**Pasul 1**: Instalează Arduino IDE sau PlatformIO

**Pasul 2**: Instalează bibliotecile necesare:

```
Adafruit GFX Library
Adafruit NeoMatrix
Adafruit NeoPixel
```

**Pasul 3**: Configurează placa ESP32:

- Board Manager → esp32 by Espressif Systems
- Selectează "ESP32 Dev Module"


### 4.2 Configurare Rețea WiFi

**Modifică în cod** (liniile 10-11):

```cpp
const char* ssid = "NUMELE_RETELEI_TALE";
const char* password = "PAROLA_RETELEI_TALE";
```


### 4.3 Upload Cod

1. Conectează ESP32 la calculator via USB
2. Selectează portul COM corespunzător
3. Upload sketch (Ctrl+U)
4. Deschide Serial Monitor (115200 baud)
5. Notează adresa IP afișată (ex: `192.168.1.100`)

## 5. UTILIZARE

### 5.1 Accesare Interfață Web

1. Conectează laptopul la aceeași rețea WiFi
2. Deschide browser și accesează: `http://ADRESA_IP_ESP32`
3. Interfața va afișa un formular cu câmp text

### 5.2 Trimitere Mesaj

1. Introdu textul dorit în câmpul "Mesaj"
2. Apasă butonul "Trimite la Panou"
3. Buzzer-ul va emite confirmarea (2 bip-uri)
4. Mesajul va apărea derulant pe matrice

### 5.3 Format URL Direct

Poți trimite mesaje direct via URL:

```
http://ADRESA_IP_ESP32/set?msg=Textul%20tau%20aici
```


## 6. PARTICULARITĂȚI TEHNICE

### 6.1 Calcul Poziție Scroll

```cpp
int textLen = scrollText.length() * 6;  // ~6 pixeli/caracter
if(--x_pos < -textLen){
  x_pos = matrix.width();  // Reset la 16 pixeli
}
```


### 6.2 Configurare NeoMatrix

Orientarea matricelor este configurată ca:

- **NEO_MATRIX_TOP + NEO_MATRIX_LEFT**: Origine în colțul stânga-sus
- **NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE**: LED-uri aranjate pe rânduri, progresiv
- **NEO_TILE_**: Repetă configurația pentru fiecare matrice 8×8


### 6.3 Consumul de Curent

**Estimare**:

- 1 LED WS2812 la maxim: ~60mA
- 128 LED-uri × 60mA = ~7.68A (la alb maxim)
- La luminozitate 20/255 (~8%), culoare roșu: ~0.8-1A

**Recomandare**: Sursa HW131 trebuie să susțină minimum 2A pentru funcționare sigură .

## 7. DEPANARE

| Problemă | Cauză Posibilă | Soluție |
| :-- | :-- | :-- |
| LED-uri nu se aprind | Level shifter neconfigurat corect | Verifică alimentarea LV (3.3V) și HV (5V) |
| Culori greșite | Ordine culori incorectă | Schimbă `NEO_GRB` în `NEO_RGB` sau `NEO_BRG` |
| ESP32 nu se conectează | Credențiale WiFi greșite | Verifică SSID și parolă în Serial Monitor |
| Buzzer continuu activ | Logică inversată | Adaugă `digitalWrite(PIN_BUZZER, LOW)` în setup |
| Server inaccesibil | Firewall sau rețea diferită | Asigură-te că dispozitivele sunt în aceeași rețea |

## 8. IMAGINI & VIDEO PREZENTARE

## 9. LICENȚĂ ȘI AUTOR

**Proiect**: Mini Billboard ESP32
**Dezvoltator**: Diaconescu Adrian
**Data**: Februarie 2026
**Hardware**: ESP32 Plusivo, WS2812 8×8

***
