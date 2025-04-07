#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "";
const char *password = "";

const char IRG_Root_X1[] PROGMEM = R"";

X509List cert(IRG_Root_X1);

WiFiServer server(80);

const int LED_PIN = 2; // Built-in LED (GPIO 2)

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Off (inverted logic)

  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client)
  {
    Serial.println("Client connected");
    String request = client.readStringUntil('\r');
    Serial.print("Request: ");
    Serial.println(request);
    client.flush();

    if (request.indexOf("GET /on") != -1)
    {
      digitalWrite(LED_PIN, LOW); // Turn on
    }
    else if (request.indexOf("GET /off") != -1)
    {
      digitalWrite(LED_PIN, HIGH); // Turn off
    }

    // Send webpage
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html><html><body>");
    client.println("<h1>ESP8266 LED Control</h1>");
    client.println("<p><a href=\"/on\"><button>Turn ON</button></a></p>");
    client.println("<p><a href=\"/off\"><button>Turn OFF</button></a></p>");
    client.println("</body></html>");

    WiFiClientSecure secureClient;

    secureClient.setInsecure();

    HTTPClient http;
    http.begin(secureClient, "https://rwherber.com/wow-market-watcher/health"); // Add endpoint for checking status

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.printf("Error in HTTP request, code: %d\n", httpCode);
      String response = http.getString();
      Serial.println(response);
    }
    http.end();

    delay(1);
    Serial.println("Client disconnected");
  }
}
