#include <ESP8266WiFi.h>
const char *ssid = "Yan Home";      //your WiFi Name
const char *password = "spiderman"; //Your Wifi Password
int ledPin = 03;
WiFiServer server(80);
void setup()
{
    Serial.begin(115200);
    delay(10);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void loop()
{
    WiFiClient client = server.available();
    if (!client)
    {
        return;
    }
    Serial.println("new client");
    
    int loopCount = 0;
    while (!client.available())
    {
        Serial.print(".");
        delay(10);
        if (++loopCount > 100)
        {
            Serial.println("Tired waiting :-(");
            return;
        }
    }
    Serial.printf("%d\n", client.available());
    client.disableKeepAlive();
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
    if (request.indexOf("/STATUS") > 0)
    {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Content-Length: 1");
        client.println("Connection: Close");
        client.println("");
        client.print(digitalRead(ledPin) == LOW ? '0' : '1');
        return;
    }
    //int value = LOW;
    if (request.indexOf("/LED") > 0)
    {

        if (request.indexOf("/LED-ON") != -1)
        {
            digitalWrite(ledPin, HIGH);
        }
        else
        {
            digitalWrite(ledPin, LOW);
        }
        client.println("HTTP/1.1 302 Moved");
        client.println("Connection: Close");
        client.println("Location: /");
        client.println("");
        return;
    }
    // Only accept index '/'
    if (request.indexOf("GET / ") < 0)
    {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: Close");
        client.println("");
        client.print("Not Found");
        return;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: Close");
    client.println("");
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    client.println("<meta http-equiv='X-UA-Compatible' content='ie=edge'>");
    client.println("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bulma@0.8.0/css/bulma.min.css'>");
    client.println("<title>LED WIFI</title>");
    client.println("</head>");
    client.println("<body>");
    client.println("<section class='section'>");
    client.println("<div class='container'>");
    client.println("<div class='card'>");
    if (digitalRead(ledPin) == LOW)
    {
        client.println("<a href='/LED-ON' class='button is-success card-footer-item'>MURUBNA</a>");
    }
    else
    {
        client.println("<a href='/LED-OFF' class='button is-danger card-footer-item'>PATENI</a>");
    }
    client.println("</div>");
    client.println("</div>");
    client.println("</section>");
    client.println("</body>");
    client.println("</html>");
    delay(1);
}