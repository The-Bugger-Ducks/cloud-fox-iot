#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "*******";
const char* password = "*******";

//String serverName = "http://cloud-fox.onrender.com/measurements/";
String serverName = "http://postman-echo.com/";

unsigned long lastTime = 0;
unsigned long timerDelay = 900000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi with IP Address: ");
  Serial.println(WiFi.localIP());
 
}

void loop() {

  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

    
    ////HTTP POST
    WiFiClient client;
    HTTPClient http_post;
    HTTPClient http;
    String url = serverName + "post";
    http_post.begin(client, url);
    Serial.println("\nPOST");
    http_post.addHeader("Content-Type", "application/json");
    http_post.addHeader("x-api-key", "qwertyuiopasdfghjklzxcvbnm");
    String data = "{\"nome\":38972142176126, \"stationId\":\"C14H\", \"measurements\":{\"heat\":24.5, \"pluv\":80},  \"momet\":\"1664967601\"}}";
    int httpCode = http_post.POST(data);
    if (httpCode > 0){
      Serial.println(httpCode);
      String payload = http_post.getString();
      Serial.print("Resposta do server: ");
      Serial.println(payload);
    } else {
      Serial.println("Http error");
    }      
    }
  
    lastTime = millis();
  }
}
