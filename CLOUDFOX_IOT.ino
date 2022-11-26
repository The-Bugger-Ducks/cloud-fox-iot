#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h> 
#include <time.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22



const char* ssid = "Rede";
const char* password = "Senha";


String serverName = "https://cloud-fox.onrender.com/measurements";
        
unsigned long lastTime = 0;

unsigned long timerDelay = 3600000; //20000

char* ntpServer = "pool.ntp.org";

unsigned long epochTime; 
struct tm timeInfo;
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = -3600*3;

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return now;
}


DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi with IP Address: ");
  Serial.println(WiFi.localIP());


  dht.begin();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {



      float h = dht.readHumidity();
      float t = dht.readTemperature();

      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }


      int pluv = random(0,5);
      int windSpeed = random(0,8);


      ////HTTP POST
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient http_post;
      epochTime = getTime();
      String url = serverName;
      
      http_post.begin(client, url);
      Serial.println("\nPOST");
      http_post.addHeader("Content-Type", "application/json");

      String data = "{\"stationId\":\"C14H\", \"measurements\":{\"temp\":\"" + String(t) + "\", \"rain\": \"" + String(pluv) +"\", \"windSpeed\": \"" + String(windSpeed) + "\", \"umi\": \"" + String(h) + "\" },  \"moment\":\"" + String(epochTime) + "\"}";

      int httpCode = http_post.POST(data);
      if (httpCode > 0)
      {
        Serial.println(httpCode);
        String payload = http_post.getString();
        Serial.print("Resposta do server: ");
        Serial.println(payload);

        
        Serial.print(F("Humidity: "));
        Serial.print(h);
        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.print(F("°C "));

      }
      else
      {
        Serial.println(httpCode);
        Serial.println("Http error");
      }
    }

    lastTime = millis();
  }
}
