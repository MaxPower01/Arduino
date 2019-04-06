// -------------------- KEYPAD 4X4 -------------------- //

#include <Arduino.h>
#include <Keypad.h>
#include <Password.h>

#define Password_Length 4

Password password = Password("1*5#3"); // password
int dlugosc = 5;                       // length of the password
int ilosc;                             // number of clicks

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[ROWS] = {6, 5, 4, 3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void checkPassword()
{
  if (password.evaluate())
  {
    int i;
    for (i = 1; i <= 2; i++)
    {
      // digitalWrite(buzzer, HIGH);
      delay(70);
      // digitalWrite(buzzer, LOW);
      delay(70);
    }
    ilosc = 0;

    Serial.println("Success");

    // digitalWrite(ledRed, LOW);
    // digitalWrite(ledGreen, HIGH);

    // lcd.clear();
    // lcd.setCursor(0, 1);
    // lcd.print(" << SUCCESS >>");
  }
  else
  {
    int i;
    for (i = 1; i <= 1; i++)
    {
      // digitalWrite(buzzer, HIGH);
      delay(200);
      // digitalWrite(buzzer, LOW);
      delay(200);
    }
    ilosc = 0;
    password.reset();

    Serial.println("Wrong");

    // digitalWrite(ledGreen, LOW);
    // digitalWrite(ledRed, HIGH);

    // lcd.clear();
    // lcd.setCursor(0, 1);
    // lcd.print("  << WRONG >>");
    delay(2000);

    // lcd.clear();
    // lcd.setCursor(1, 0);
    // lcd.print("  :Welcome:");
    // lcd.setCursor(0, 1);
    // lcd.print("PLEASE ENTER PIN");
  }
}

void keypadEvent(KeypadEvent eKey)
{
  switch (keypad.getState())
  {
  case PRESSED:

    int i;
    for (i = 1; i <= 1; i++)
    {
      // digitalWrite(buzzer, HIGH);
      delay(50);
      // digitalWrite(buzzer, LOW);
      delay(50);
    }

    Serial.print("Pressed: ");
    Serial.println(eKey);

    switch (eKey)
    {
      /*
case '#':
break;
 
case '*':
break;
*/

    default:
      ilosc = ilosc + 1;
      password.append(eKey);
    }
    //Serial.println(ilosc);

    if (ilosc == 1)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("*_");
    }
    if (ilosc == 2)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("**_");
    }
    if (ilosc == 3)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("***_");
    }
    if (ilosc == 4)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("****_");
    }
    if (ilosc == 5)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("*****_");
    }
    if (ilosc == 6)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("******_");
    }
    if (ilosc == 7)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("*******_");
    }
    if (ilosc == 8)
    {
      // lcd.clear();
      // lcd.setCursor(1, 0);
      // lcd.print("   < PIN >");
      // lcd.setCursor(0, 1);
      // lcd.print("********");
    }

    if (ilosc == dlugosc)
    {
      delay(250);
      checkPassword();
      ilosc = 0;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  keypad.addEventListener(keypadEvent);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  keypad.getKey();
}
