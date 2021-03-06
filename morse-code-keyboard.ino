
#include <Keyboard.h>

//Pin + Time Definitions
#define INPUTPIN 3 //Pin where button inputs are directed
#define TIMEPERIODMS 50 //Morse time period (<3 = dit, >3 = dash)
#define AFKTIMEMS 10000 //Time until you are considered AFK

//Get a morse code character from the transmission length
char getChar(unsigned long chartime)
{
  if(chartime < 3 * TIMEPERIODMS)
  {
    return '.';
  }
  else
  {
    return '-';
  }
}

//Return corresponding character from a char array of morse code characters
char charFromMorse(char morseArray[])
{
  String morseString = morseArray;
  morseString.trim();
  
  String morseChars[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", // a-ö
  ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "-----", // 0-9
  ".-.-.-", "--..--", "..--..", ".----.", "-.-.--", "-..-.", "-.--.", "-.--.-", ".-...", "---...", "-.-.-.", "-...-", ".-.-.", "-....-", "..--.-", ".-..-.", "...-..-", ".--.-.", "........" //Punctuation
  };

  char correspondentChars[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
  '1','2','3','4','5','6','7','8','9','0',
  '.', ',', '?', '\'', '!', '/', '(', ')', '&', ':', ';', '=', '+', '-', '_', '"', '$', '@', 8
  };

  for(int i = 0; i < sizeof(morseChars); i++)
  {
    if(morseString == morseChars[i])
    {
      return correspondentChars[i];
    }
  }

  return '*';
}

void setup() 
{
  Keyboard.begin();
  pinMode(INPUTPIN, INPUT);
}

//Global Variables used in loop()
unsigned long pressBegin;
boolean transmissionStartChar = true; //Boolean to check if the character being entered is the first character
char morse[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}; //Main array containing morse code characters
int morsePointer = 0; //Pointer referencing elements in morse[]

void loop() 
{   
  unsigned long timeSinceLastPress = millis(); //Get the time since the button was last pressed

  //Pushes a morse code character to morse[] when the button is pressed
  if(digitalRead(INPUTPIN) == HIGH)
  {
    if(timeSinceLastPress - pressBegin > TIMEPERIODMS * 15 && !transmissionStartChar && timeSinceLastPress - pressBegin < AFKTIMEMS)
    {
      Keyboard.press(' ');
      Keyboard.releaseAll();
    }
    
    pressBegin = millis();
    transmissionStartChar = false;
    
    while(digitalRead(INPUTPIN) == HIGH)
    {
      delay(TIMEPERIODMS/2); //For debouncing (i know its jank ok)
      
      if(digitalRead(INPUTPIN) == LOW)
      {
        unsigned long pressEnd = millis();
        unsigned long pressTime = pressEnd - pressBegin;

        morse[morsePointer] = getChar(pressTime);
        morsePointer++;
        
        if(morsePointer == 8)
        {
          Keyboard.press(charFromMorse(morse));
          Keyboard.releaseAll();
          for(int i = 0; i < sizeof(morse); i++)
          {
            morse[i] = ' ';
          }
          morsePointer = 0;
        }
      }
    }
  }

  if(timeSinceLastPress - pressBegin > TIMEPERIODMS * 12 && morse[0] != ' ')
  {
    Keyboard.press(charFromMorse(morse));
    Keyboard.releaseAll();
    for(int i = 0; i < sizeof(morse); i++)
    {
      morse[i] = ' ';
    }
    morsePointer = 0;
    pressBegin = millis();
  }
}
