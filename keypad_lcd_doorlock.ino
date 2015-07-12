
/*
  Keypad and lcd door unlocker using password library.

  3x4 keypad and 20x4 lcd display. accepts incoming code and 
  displays current character on screen. when correct code has been entered 
  (comparison done using the password library) it triggers two outputs to 
  open a relay or other device
  
  The circuit:
  * 3x4 numerical keypad connected to pins 0,1,2,3,A0,A1,A2
  * 20x4 lcd display connected to pins 7,8,9,10,11,12
  * red led on pin 5
  * green led on pin 6
  * white led (to be a transistor or relay to trigger door opener) pin A3
  * Arduino Uno 
  * 
  *   LCD Screen
  *   
  *   VSS > Gnd
  *   VDD > +5v
  *   V0 > 3900 ohm resistor > Gnd
  *   RS > Pin 7
  *   RW > GND
  *   E > Pin 8
  *   D4 > Pin 9
  *   D5 > Pin 10
  *   D6 > Pin 11
  *   D7 > Pin 12
  *   A > +5V
  *   K > GND
  *   

  *   Keypad > Arduino Pins (mapping for yours may be different check the spec)
  *   
  *   Pin 1 > A1
  *   Pin 2 > Pin 0
  *   Pin 3 > A0
  *   Pin 4 > Pin 3
  *   Pin 5 > A2
  *   Pin 6 > Pin 2
  *   Pin 7 > Pin 1
  *   
  *   
  

  Created 12 Jul 2015
  By Siparker

  inspiration and some code taken from 
  https://tech-zen.tv/episodes/shows/make-it-work/episodes/keypad-input-to-an-arduino-episode-11
  http://www.instructables.com/id/Password-access-with-arduino/step5/The-code-and-the-end/
  http://playground.arduino.cc/Code/Password
  https://learn.adafruit.com/character-lcds/overview

  I have used a 6 char password for extra security. you can use whatever 
  length you like but you will need to update the line in loop where the 
  password index is checked to whatever length you want to use. I will
  be updating this to check the set password length and apply that but for
  now it works well and will be used to control the 12v strike plate on my 
  door for simple password entry 

*/



#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Password.h>

Password password = Password( "123456" ); // set password using the password library
int currentPosition = 7; // set initial lcd position for displaying password on screen

// initialise LCD display
const int ledRows= 4;
const int ledCols= 20;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Initialise keypad and generat keymapping
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

// Set Keypad Pins
byte rowPins[ROWS] = { 0, 1, 2, 3 };
byte colPins[COLS] = { 14, 15, 16 };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Set LED pins
int RedpinLock = 5;
int GreenpinUnlock = 6;
int PowerUnlock = 17;

// Initialise Password Index Variable
int passwordIndex = 0;

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  // Display the entry screen on the lcd.
  displayCodeEntryScreen();

// set initial led statuses  
pinMode(RedpinLock, OUTPUT);
digitalWrite(RedpinLock, LOW);
pinMode(GreenpinUnlock, OUTPUT);
digitalWrite(GreenpinUnlock, LOW);
pinMode(PowerUnlock, OUTPUT);
digitalWrite(PowerUnlock, LOW);
}



void loop(){
  // get key when pressed
  char key = keypad.getKey();

  //when key is pressed add to the password compare and increase index. output entered char as a * onto screen
  if (key){
    password.append(key);
    passwordIndex++;
    lcd.setCursor(currentPosition,2);
    lcd.print("*");
    currentPosition++;
    delay(100);
    // repeat until password index = 6 which is the length of my password at the top of the sketch
    if(passwordIndex==6)
    {
      passwordIndex = 0;
      currentPosition = 7;
      guessPassword();
      
    }
  }
}

void displayCodeEntryScreen()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("**** Enter Code ****");
  lcd.setCursor(0,1);
  lcd.print("********************");
  lcd.setCursor(0,2);
  lcd.print("**                **");
  lcd.setCursor(0,3);
  lcd.print("********************");
}

void invalidCode()
// Flashes the invalid code message and red led. then flashes red led quickly to indicate reset.
{
  for (int i=0; i <= 5; i++){
      digitalWrite(RedpinLock, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("** ACCESS DENIED! **");
  lcd.setCursor(0,2);
  lcd.print("**  INVALID CODE  **");
  lcd.setCursor(0,3);
  lcd.print("********************");
      delay(500);
  digitalWrite(RedpinLock, LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("**                **");
  lcd.setCursor(0,2);
  lcd.print("**                **");
  lcd.setCursor(0,3);
  lcd.print("********************");
  delay(500);
   } 
  
  digitalWrite(RedpinLock, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("** ACCESS DENIED! **");
  lcd.setCursor(0,2);
  lcd.print("**  INVALID CODE  **");
  lcd.setCursor(0,3);
  lcd.print("********************");
  for (int i=0; i <= 5; i++){
    digitalWrite(RedpinLock, HIGH);
    delay(100);
    digitalWrite(RedpinLock, LOW);
    delay(100);
  }
  
  displayCodeEntryScreen();
}

void unlockDoor()
{
digitalWrite(GreenpinUnlock, HIGH);

// sets second output to high so LED can be triggered seperately if a mosfet etc is required to trigger a door strike plate or a relay etc.
digitalWrite(PowerUnlock, HIGH);
lcd.clear();
lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("** ACCESS GRANTED **");
  lcd.setCursor(0,2);
  lcd.print("**   WELCOME!!    **");
  lcd.setCursor(0,3);
  lcd.print("********************");

//add any code to unlock the door here
  delay(5000);

  // set green led and power unlock pin to low and show entry screen again.
  digitalWrite(GreenpinUnlock, LOW);
  digitalWrite(PowerUnlock, LOW);
  displayCodeEntryScreen();

}

void guessPassword(){
if (password.evaluate()){
  unlockDoor();
}
else{
invalidCode();
}
}
