
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Animation_Frames.h"

// Because the two eye matrices share the same address, only four
// matrix objects are needed for the five displays:
#define MATRIX_EYES         0
#define MATRIX_MOUTH_LEFT   1
#define MATRIX_MOUTH_MIDDLE 2
#define MATRIX_MOUTH_RIGHT  3
Adafruit_8x8matrix matrix[4] = { // Array of Adafruit_8x8matrix objects
  Adafruit_8x8matrix(), Adafruit_8x8matrix(),
  Adafruit_8x8matrix(), Adafruit_8x8matrix() };

// Rather than assigning matrix addresses sequentially in a loop, each
// has a spot in this array.  This makes it easier if you inadvertently
// install one or more matrices in the wrong physical position --
// re-order the addresses in this table and you can still refer to
// matrices by index above, no other code or wiring needs to change.
static const uint8_t matrixAddr[] = { 0x70, 0x71, 0x72, 0x73 };


uint8_t
  blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, // Blink bitmap sequence
  blinkCountdown = 100, // Countdown to next blink (in frames)
  gazeCountdown  =  75, // Countdown to next eye movement
  gazeFrames     =  50, // Duration of eye movement (smaller = faster)
  mouthPos       =   0, // Current image number for mouth
  mouthCountdown =  10; // Countdown to next mouth change
int8_t
  eyeX = 3, eyeY = 3;   // Current eye position
/*  newX = 3, newY = 3,   // Next eye position
  dX   = 0, dY   = 0;   // Distance from prior to new position
*/
uint8_t
  pupilX  = 3, pupilY = 3,
  pupilSize = 2;


void setup() {

  // Seed random number generator from an unused analog input:
  randomSeed(analogRead(A0));
  Serial.begin(9600);

  uint8_t postFail = 0;
  
  // Initialize each matrix object and run POST
  Serial.println("Starting power-on self-test.    ");
  for(uint8_t i=0; i<4; i++) {
    matrix[i].begin(matrixAddr[i]);
    matrix[i].setRotation(3);
    matrix[i].setBrightness(0);
  
    postFail = post(matrix[i]);

    if (postFail){
      Serial.print("matrix ");
      Serial.print(i);
      Serial.println(" failed power-on slef-test! This is fatal, exiting.");
      delay(500); //give async serial time to make it out to console
      exit(1);
    }else {
      Serial.print("Power-on self-test passed for matrix: ");
      Serial.println(i);
    }
  }
}

void loop() {      
  // always be blinking    
  matrix[MATRIX_EYES].clear();
  matrix[MATRIX_EYES].drawBitmap(0, 0,
  blinkImg[(blinkCountdown < sizeof(blinkIndex)) ? blinkIndex[blinkCountdown] : 0], 8, 8, LED_ON);
  if(--blinkCountdown == 0) blinkCountdown = random(5, 180);

  // make a pupil by filling a rectangle with dark pixels
  matrix[MATRIX_EYES].fillRoundRect(pupilX, pupilY, pupilSize, pupilSize, 1, LED_OFF);
  for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();
  delay(20); // ~50 FPS

  if(Serial.available()){
    char inByte = Serial.read();
    switch (inByte){
      case 'w':      pupilY++;     break; //still need bounds checking on these suckers!
      case 'a':      pupilX++;     break;
      case 's':      pupilY--;     break;
      case 'd':      pupilX--;     break;
      case 'f':      pupilSize--;  break;// pupil gets smaller
      case 'g':      pupilSize++;  break;//pupil gets bigger
      case 'r':      // fall through to reset
      default:  pupilX = pupilY = 3; pupilSize = 2; break;
    }
  }
}


int post(Adafruit_8x8matrix matrix) {
  // Right now this test always passes and it is up to the user to observe the output
  // to determine if overything went OK
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(500);

  matrix.clear();
  matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(500);

  matrix.clear();
  matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(500);

  matrix.clear();      // clear display
  matrix.drawPixel(0, 0, LED_ON);  
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawLine(0,0, 7,7, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawRect(0,0, 8,8, LED_ON);
  matrix.fillRect(2,2, 4,4, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawCircle(3,3, 3, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  for (int8_t x=0; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("Hello");
    matrix.writeDisplay();
    delay(100);
  }
  return(0);
}  
 
  
