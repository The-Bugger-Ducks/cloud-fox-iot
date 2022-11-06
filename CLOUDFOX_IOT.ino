#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h> 
#include <time.h>

const char* ssid = "Nome da rede";
const char* password = "Sua senha";

String serverName = "https://cloud-fox.onrender.com/measurements";
        
unsigned long lastTime = 0;
unsigned long timerDelay = 3600000;

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

void setup()
{
  Serial.begin(115200);

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

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {

      int pluv = random(0,50);
      int temp = random(16,30);
      int windSpeed = random(0,8);
      int umi = random(1,96);

      ////HTTP POST
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient http_post;
      epochTime = getTime();
      String url = serverName;
      
      http_post.begin(client, url);
      Serial.println("\nPOST");
      http_post.addHeader("Content-Type", "application/json");
      String data = "{\"stationId\":\"C14H\", \"measurements\":{\"temp\":\"" + String(temp) + "\", \"pluv\": \"" + String(pluv) +"\", \"windSpeed\": \"" + String(windSpeed) + "\", \"umi\": \"" + String(umi) + "\" },  \"moment\":\"" + String(epochTime) + "\"}";
      int httpCode = http_post.POST(data);
      if (httpCode > 0)
      {
        Serial.println(httpCode);
        String payload = http_post.getString();
        Serial.print("Resposta do server: ");
        Serial.println(payload);
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
