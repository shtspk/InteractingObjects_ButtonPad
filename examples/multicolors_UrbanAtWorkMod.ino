/* This is a temporary fork to help a student that is building a project 

changes/additions: 

    1. added variables that match the sparkfun board to help with the wiring
    2. added some Serial.print output to display which button is being pressed and how

  BIG thanks to jsiobj for creating the library and noticing the pin/freq issues.
  
*/


/*=====================================================================================

  This is an example which makes uses of InteractingObjects_RGBLedMatrix library

  For an assembly and connection tutorial see 
  - http://interactingobjects.com/sparkfuns-4x4-button-led-matrix-part-1-soldering/
  - http://interactingobjects.com/sparkfuns-4x4-button-led-matrix-part-2-connecting-to-an-arduino-mega/ 
  - http://interactingobjects.com/sparkfuns-4x4-button-led-matrix-part-3-playing-with-the-pad
  
  WARNING : As of now, in the tutorial, I use PWM pins 2 to 13. But I swapped pins
            4 a 13 respectively with pin 44 & 45.
        
            Q:Why didn't I used pin 4 & 13 and 44 & 45 instead ? 
            
            A:Because I needed to increase PWM frequency to make the matrix works.
              To do this, you need to change Timers config. Pins 4 and 13 use timer 0.
              But... as it is used by Arduino's core timing functions,  you don't want 
              to mess with it. Do you ?


  The MIT License (MIT)
  
  Copyright (c) 2014 Interacting Objects (http://interactingobjects.com)
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ======================================================================================*/

#include <InteractingObjects_ButtonPad.h>
#include <Keypad.h>

/* ++++++++++++++++++++++++++++++
    UrbanAtWork comment:
    I have added variables to help with the wiring. 
    The variable names nearly match the ones on the board.
*/

const byte LED_GND4 = 46;
const byte SWT_GND4 = 48;
const byte LED_GND3 = 50;
const byte SWT_GND3 = 52;

const byte LED_GND2 = 31;
const byte SWT_GND2 = 27;
const byte LED_GND1 = 30;
const byte SWT_GND1 = 26;

const byte GREEN4 = 45;
const byte BLUE4 = 12;
const byte RED4 = 11;
const byte SWITCH4 = 23;

const byte GREEN3 = 10;
const byte BLUE3 = 9;
const byte RED3 = 8;
const byte SWITCH3 = 38;

const byte GREEN2 = 7;
const byte BLUE2 = 6;
const byte RED2 = 5;
const byte SWITCH2 = 37;

const byte GREEN1 = 44;
const byte BLUE1 = 3;
const byte RED1 = 2;
const byte SWITCH1 = 36;


// Define pin used for buttons
byte pinBtn[ROWS]       = {SWITCH1,SWITCH2,SWITCH3,SWITCH4}; // Buttons rows
byte pinBtnGnd[COLS]    = {SWT_GND1,SWT_GND2,SWT_GND3,SWT_GND4}; // Buttons cols

// Define pins used for RGB leds
byte customPinLedRGB[ROWS][3] = {{RED1,BLUE1,GREEN1},{RED2,BLUE2,GREEN2},{RED3,BLUE3,GREEN3},{RED4,BLUE4,GREEN4}}; // RGB pins
byte customPinLedGnd[COLS]    = {LED_GND1,LED_GND2,LED_GND3,LED_GND4};                          // GND pins

// "Standard" colors                        
#define COLOR_RED     0
#define COLOR_GREEN   1
#define COLOR_BLUE    2
#define COLOR_YELLOW  3
#define COLOR_MAGENTA 4
#define COLOR_CYAN    5
#define COLOR_WHITE   6
#define COLOR_OFF     7

byte standardColors[][3] =  { 
  {255,0  ,0  }, 
  {0  ,255,0  }, 
  {0  ,0  ,255},
  {255,255,0  },
  {255,0  ,255},
  {0  ,255,255},
  {255,255,255},
  {0  ,0  ,0  }
};

// For keypad
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};

// Creating keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), pinBtn, pinBtnGnd, ROWS, COLS); 

// Creating led matrix
rgbLedMatrix ledMatrix = rgbLedMatrix(customPinLedRGB,customPinLedGnd,ROWS,COLS);

//-------------------------------------------------------------------------
// Reconfiguration of PWM frequency
// Thanks to http://letsmakerobots.com/profile/oddbot
// See http://letsmakerobots.com/node/40136 
//-------------------------------------------------------------------------
void setPWMFreq() {
  
  TCCR0B = TCCR0B & B11111000 | B00000011;    // D4 & D13 - Keeping default because Timer0 is used for Arduino timing function ike millis(), delay()...
  
  // set timer 1,2,3,4,5 divisor to 1 for PWM frequency of 31372.55 Hz
  TCCR1B = TCCR1B & B11111000 | B00000001;    // D11 & D12
  TCCR2B = TCCR2B & B11111000 | B00000001;    // D9 & D10 - Used by tone()... but I don't use tone() so I don't care !
  TCCR3B = TCCR3B & B11111000 | B00000001;    // D2, D3 & D5
  TCCR4B = TCCR4B & B11111000 | B00000001;    // D6, D7 & D8
  TCCR5B = TCCR5B & B11111000 | B00000001;    // D44, D45 & D46
}
//-------------------------------------------------------------------------
// What to do when a key is pressed
//-------------------------------------------------------------------------
void onKeyPressed(char keyCode) {
  byte row=keyCode/4;
  byte col=keyCode%4;
  
  ledMatrix.matrixLedSetRandom(row,col); // Switch the led on with a randow color or changing the color if on  
  ledMatrix.matrixLedUnlock(row,col);    // ... and "unlock" the led, i.e. when button is release, switch it off
}

//-------------------------------------------------------------------------
// What to do when a key is held
//-------------------------------------------------------------------------
void onKeyHold(int keyCode) {
  byte row=keyCode/4;
  byte col=keyCode%4;

  // When a button is held, "locking" the color (i.e. don't switch led off when key is released
  if(ledMatrix.matrixLedGetState(row,col)==ON) ledMatrix.matrixLedLock(row,col);  
}

//-------------------------------------------------------------------------
// What to do when a key is released
//-------------------------------------------------------------------------
void onKeyReleased(int keyCode) {
  byte row=keyCode/4;
  byte col=keyCode%4;

  // When the key is released, if not locked (i.e. key was not hold) and led is ON, switching it OFF
  if(ledMatrix.matrixLedGetLockState(row,col)==OFF && ledMatrix.matrixLedGetState(row,col)==ON)
    ledMatrix.matrixLedSetState(row,col,standardColors[COLOR_OFF]);

}

//-------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Starting up... ");
  setPWMFreq();     // Setting a higher frequency for PWM
  
  // Just testing leds
  ledMatrix.ledTestAll(standardColors[COLOR_RED]);
  ledMatrix.ledTestAll(standardColors[COLOR_GREEN]);
  ledMatrix.ledTestAll(standardColors[COLOR_BLUE]);
  ledMatrix.ledTestAll(standardColors[COLOR_WHITE]);
  
  // Switching on all leds with random colors
  ledMatrix.matrixLedSetAllRandom();

}

//-------------------------------------------------------------------------
void loop() {
  
  // At least one key state changed
  if (customKeypad.getKeys()){

    for (int i=0; i<LIST_MAX; i++)  {  // Go through 10 keys max
        if ( customKeypad.key[i].stateChanged ) { // Check if state changed
          switch (customKeypad.key[i].kstate) {         // Report active key state : IDLE, PRESSED, HOLD, or RELEASED

                case PRESSED:
                  Serial.print("Key ");
                  Serial.print(customKeypad.key[i].kcode);
                  Serial.println(" Pressed");
                  onKeyPressed(customKeypad.key[i].kcode);
                  break;
                
                case HOLD:
                  Serial.print("Key ");
                  Serial.print(customKeypad.key[i].kcode);
                  Serial.println(" Held");
                  onKeyHold(customKeypad.key[i].kcode);
                  break;
                
                case RELEASED:
                  Serial.print("Key ");
                  Serial.print(customKeypad.key[i].kcode);
                  Serial.println( " Released");
                  onKeyReleased(customKeypad.key[i].kcode);
                  break;
                
                case IDLE:
                  // do nothing !
                  break;
          }
        }
    }
  }

  // refreshing the led matrix (all we did up until now is updating the status "logically")
  ledMatrix.matrixLedRefresh(500);
}
