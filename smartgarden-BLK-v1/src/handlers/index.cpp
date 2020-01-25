#include "web.h"

void handle_index(String &response, HTTPMethod method)
{
    //digitalWrite(ledPin, LOW);
    File f = SPIFFS.open("/index.html.gz", "r");
    server.streamFile(f, "text/html");
    f.close();
    //digitalWrite(ledPin, HIGH);
}
