#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "unibuc_free_7";
const char* password = "adrian2511";

#define PIN_LED 13
#define PIN_BUZZER 14
#define MAT_W 8
#define MAT_H 8
#define TILES_X 2
#define TILES_Y 1

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MAT_W, MAT_H, TILES_X, TILES_Y, PIN_LED,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE +
  NEO_TILE_TOP       + NEO_TILE_LEFT + NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

WebServer server(80);

String scrollText = "Holla!";
int x_pos;
int minX;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Billboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; background-color: #222; color: white; padding-top: 50px; }
    input[type=text] { padding: 10px; font-size: 20px; width: 80%; margin-bottom: 20px; border-radius: 5px; }
    input[type=submit] { background-color: #4CAF50; color: white; padding: 15px 32px; font-size: 20px; border: none; cursor: pointer; border-radius: 5px;}
  </style>
</head>
<body>
  <h1>Panou Control LED</h1>
  <form action="/set" method="GET">
    Mesaj: <input type="text" name="msg">
    <br>
    <input type="submit" value="Trimite la Panou">
  </form>
</body>
</html>
)rawliteral";

void setup() {
  
  Serial.begin(115200);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  matrix.setTextColor(matrix.Color(255, 0, 0));

  x_pos = matrix.width();
  minX = -12 * 6;

  WiFi.begin(ssid, password);
  Serial.print("Conectam la WiFi");
  while(WiFi.status() != WL_CONNECTED){

    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Adresa IP Server: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", index_html);
  });

  server.on("/set", HTTP_GET, []() {

    if(server.hasArg("msg")){
      
      scrollText = server.arg("msg");
      Serial.println("Mesaj nou: " + scrollText);

      digitalWrite(PIN_BUZZER, HIGH);
      delay(100);
      digitalWrite(PIN_BUZZER, LOW);
      delay(50);
      digitalWrite(PIN_BUZZER, HIGH);
      delay(100);
      digitalWrite(PIN_BUZZER, LOW);

      x_pos = matrix.width();

      server.send(200, "text/html", "<h1>Mesaj Trimis! <a href='/'>Inapoi</a></h1>");
    }else{
      server.send(400, "text/plain", "Lipseste mesajul");
    }

  });

  server.begin();

}

void loop() {
  
  server.handleClient();

  static unsigned long lastUpdate = 0;

  if(millis() - lastUpdate > 80){

    lastUpdate = millis();
    matrix.fillScreen(0);
    matrix.setCursor(x_pos, 0);
    matrix.print(scrollText);

    int textLen = scrollText.length() * 6;
    
    if(--x_pos < -textLen){
      x_pos = matrix.width();
    }

    matrix.show();

  }

}
