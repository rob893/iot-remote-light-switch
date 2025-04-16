#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

/**
Under Tools > Board:
Choose NodeMCU 1.0 (ESP-12E Module)

Under Tools > Port: com 3
**/

const char *ssid = "";
const char *password = "";

const int LED_PIN = 2; // Built-in LED (GPIO 2)
unsigned long lastPollTime = 0;
const unsigned long pollInterval = 60000; // Poll every minute (60000ms)
const char *statusEndpoint = "https://rherber-iotlightswitch-api-uw-wa-d.azurewebsites.net/lightSwitchState";
const char *callbackEndpoint = "https://rherber-iotlightswitch-api-uw-wa-d.azurewebsites.net/lightSwitchState/deviceCallback";

bool previousState = false; // Track the previous LED state

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
}

void reportStateChange(bool success, const char *errorMessage, bool attemptedState)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClientSecure secureClient;
    secureClient.setInsecure();

    HTTPClient http;
    http.begin(secureClient, callbackEndpoint);
    http.addHeader("Content-Type", "application/json");

    // Create JSON document
    StaticJsonDocument<200> doc;
    doc["success"] = success;
    doc["errorMessage"] = errorMessage;
    doc["attemptedState"] = attemptedState;

    // Serialize JSON to string
    String jsonString;
    serializeJson(doc, jsonString);

    int httpCode = http.POST(jsonString);
    if (httpCode != HTTP_CODE_OK)
    {
      Serial.printf("Error reporting state change, code: %d\n", httpCode);
    }
    http.end();
  }
}

// Helper function to set LED state and return success
bool setLedState(bool targetState)
{
  if (targetState)
  {
    digitalWrite(LED_PIN, LOW); // Turn on
    Serial.println("LED turned ON");
  }
  else
  {
    digitalWrite(LED_PIN, HIGH); // Turn off
    Serial.println("LED turned OFF");
  }
  return true; // On Arduino, digitalWrite doesn't fail unless there's a hardware issue
}

void loop()
{
  unsigned long currentTime = millis();

  // Check if it's time to poll (every minute)
  if (currentTime - lastPollTime >= pollInterval)
  {
    lastPollTime = currentTime;

    if (WiFi.status() == WL_CONNECTED)
    {
      WiFiClientSecure secureClient;
      secureClient.setInsecure();

      HTTPClient http;
      http.begin(secureClient, statusEndpoint);

      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString();
        Serial.print("Light state from API: ");
        Serial.println(payload);

        bool success = true;
        const char *errorMessage = "";
        bool targetState = (payload == "true");

        // Only proceed if the target state is different from previous state
        if (targetState != previousState)
        {
          Serial.println("State change detected, updating LED...");

          // Set the LED state and check result
          success = setLedState(targetState);

          if (!success)
          {
            errorMessage = "Failed to set LED state";
            reportStateChange(false, errorMessage, targetState);
          }
          else
          {
            previousState = targetState; // Update previous state only on successful change
            reportStateChange(true, "", targetState);
          }
        }
        else
        {
          Serial.println("No state change needed");
        }
      }
      else
      {
        Serial.printf("Error in HTTP request, code: %d\n", httpCode);
        String response = http.getString();
        Serial.println(response);

        // Report the failed attempt, but only if it would have been a state change
        bool targetState = previousState; // We don't know the intended state, use previous
        reportStateChange(false, "Failed to get state from API", targetState);
      }
      http.end();
    }
    else
    {
      Serial.println("WiFi disconnected, attempting to reconnect...");
      WiFi.begin(ssid, password);
    }
  }

  // Small delay to prevent overwhelming the CPU
  delay(100);
}
