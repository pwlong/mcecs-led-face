
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Animation_Frames.h"



// Because the two eye matrices share the same address, only four
// matrix objects are needed for the five displays:
#define EYES         0
#define MOUTH_LEFT   1
#define MOUTH_MIDDLE 2
#define MOUTH_RIGHT  3
///#define MATRICES     2
//String  message = "Hello World!";

Adafruit_8x8matrix matrix[4] = { // Array of Adafruit_8x8matrix objects
  Adafruit_8x8matrix(), Adafruit_8x8matrix(),
  Adafruit_8x8matrix(), Adafruit_8x8matrix() };

// Rather than assigning matrix addresses sequentially in a loop, each
// has a spot in this array.  This makes it easier if you inadvertently
// install one or more matrices in the wrong physical position --
// re-order the addresses in this table and you can still refer to
// matrices by index above, no other code or wiring needs to change.
static const uint8_t matrixAddr[] = { 0x70, 0x72, 0x71, 0x73, 0x74, 0x75, 0x76, 0x77};


uint8_t
  blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, // Blink bitmap sequence
  blinkCountdown = 100, // Countdown to next blink (in frames)
  gazeCountdown  =  75, // Countdown to next eye movement
  gazeFrames     =  50, // Duration of eye movement (smaller = faster)
  mouthPos       =   0, // Current image number for mouth
  mouthCountdown =  10; // Countdown to next mouth change
//int8_t
//  eyeX = 3, eyeY = 3;   // Current eye position
  
uint8_t
  pupilX  = 3, pupilY = 3,
  pupilSize = 2;


void setup() {
  // Seed random number generator from an unused analog input:
  randomSeed(analogRead(A0));
  Serial.begin(9600);
  pinMode(13,OUTPUT);

  uint8_t postFail = 0;
 

  // Initialize each matrix object and run POST
  Serial.println("Starting power-on self-test.");
  for(uint8_t i=0; i<8; i++) {
    matrix[i].begin(matrixAddr[i]);
    matrix[i].setTextWrap(false);
    matrix[i].setRotation(3);
    matrix[i].setBrightness(15);
	// uncomment this line and comment out the next to skip post for testing postFail = 0;
    //postFail = post(matrix[EYES]);

    
	// This is kind of ridiculous right now, POST always passes as far as the code is concerned
	// it is up to the user to observe the output and decide if everything is working
	// This stuff in in here in case the next programmer is better than pwl and he/she can
	// figure out a way to have the LEDs actually report if they have passed or failed
	// and also to prompt future students to understand that self testing is something we
	// are trying to get everyone to do.
  if (postFail){
      Serial.print("matrix ");
      Serial.print(i);
      Serial.println(" failed power-on slef-test! This is fatal, exiting.");
      delay(900); //give async serial time to make it out to console
      exit(1);
    }else {
      Serial.print("Power-on self-test passed for matrix: ");
      Serial.println(i);
    }
  }
  Serial.println("Starting normal Operation");




  // srcoll POST message across the mouth
  for (int x=0; x>=-176*6; x--) {
   matrix[3].clear();
   matrix[3].setCursor(x+8,1);
   matrix[3].print("Hello Dr. Perkowski. Would you like to hear a joke?              OK   A preist a rabbi and a nun walk into a bar......Actually  maybe I better tell you after class.");
   matrix[3].writeDisplay();
   
   matrix[2].clear();
   matrix[2].setCursor(x+16,1);
   matrix[2].print("Hello Dr. Perkowski. Would you like to hear a joke?              OK   A preist a rabbi and a nun walk into a bar......Actually  maybe I better tell you after class.");
   matrix[2].writeDisplay();
   
   matrix[1].clear();
   matrix[1].setCursor(x+24,1);
   matrix[1].print("Hello Dr. Perkowski. Would you like to hear a joke?              OK   A preist a rabbi and a nun walk into a bar......Actually  maybe I better tell you after class.");
   matrix[1].writeDisplay();
   delay(20);
  } 
}

void loop() {  

  char buffer[80]; 
  int rc;
  
  // always be blinking
  matrix[EYES].clear();
  matrix[EYES].drawBitmap(0, 0, blinkImg[(blinkCountdown < sizeof(blinkIndex)) ? blinkIndex[blinkCountdown] : 0], 8, 8, LED_ON);
  if(--blinkCountdown == 0) blinkCountdown = random(5, 180);

// Draw mouth, switch to new random image periodically
  drawMouth(mouthImg[mouthPos]);
  if(--mouthCountdown == 0) {
    mouthPos = random(6); // Random image
    // If the 'neutral' position was chosen, there's a 1-in-5 chance we'll
    // select a longer hold time.  This gives the appearance of periodic
    // pauses in speech (e.g. between sentences, etc.).
    mouthCountdown = ((mouthPos == 0) && (random(5) == 0)) ?
      random(200, 800) : // Longer random duration  -----PWL original values (10,40)
      random(2, 8);    // Shorter random duration -----PWL original values ( 2, 8)
  }





  // make a pupil by filling a rectangle with dark pixels
  matrix[EYES].fillRoundRect(pupilX, pupilY, pupilSize, pupilSize, 1, LED_OFF);
  for(uint8_t i=0; i<4; ++i) matrix[i].writeDisplay();
  delay(20); // ~50 FPS

  if(Serial.available()){
	if (readline(Serial.read(), buffer, 80) >0) {
	  Serial.print("you sent over: ");
	  Serial.println(buffer);
	  if (strncmp(buffer,"topdown",80) == 0){
		rc = frame_4(topdown);
	  } else
	  if (strncmp(buffer, "squint", 80) == 0){
		rc = frame_4(squint);
	  } else
	  if (strncmp(buffer, "botup", 80) == 0){
		rc = frame_4(botup);
	  } else
	  if (strncmp(buffer, "angler", 80) == 0){
		rc = frame_4(angler);
	  } else
	  if (strncmp(buffer, "curve", 80) == 0){
		rc = frame_4(curve);
		
		
		
	    }else {
	      Serial.print("I don't know what that means\n");
	    }
      }

	  
}


/*
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
*/

}
