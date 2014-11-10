// Helper functions for the py_eyes sketch

int post(Adafruit_8x8matrix matrix) {
  // Right now this test always passes and it is up to the user to observe the output
  // to determine if overything went OK
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  for (int8_t x=0; x>=-36; x--) {
    digitalWrite(13,HIGH);
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("Hello");
    matrix.writeDisplay();
    digitalWrite(13,LOW);
    delay(100); 
  }
  return(0); 
}




int readline(int readch, char *buffer, int len){
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}

  
