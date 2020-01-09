
int ledPin = 03;
int btnPin = 12;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT);
}

void loop()
{
  int buttonValue = digitalRead(btnPin);
  if (buttonValue == HIGH)
  {
    // If button pushed, turn LED on
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    // Otherwise, turn the LED off
    digitalWrite(ledPin, LOW);
  }
  
}
