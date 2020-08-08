uint16_t currentLEDs(uint16_t y){
  
  uint16_t currentLEDs = initialLEDs;
  
  for (int k = 0; k < y + 1; k++){
    currentLEDs += steps[k];
  }
  return currentLEDs;
}

uint16_t tLEDs(uint16_t y){
  
  int16_t tLEDs = - 1;
  
  for (int k = 0; k < y + 1; k++){
    tLEDs += currentLEDs(k);  
  }
  
  return tLEDs;
}

/* This function takes x, y coordinates as imputs and gives out the corresponding index of the led, similiar to a normal matrix. 
A third parameter is "richtung" which is a bool that determines is the leds are addressed from left to right (true) or right to left (false)
*/

uint16_t xyAll( uint8_t x, uint8_t y, bool richtung = true){
  uint16_t i;
  bool gerade = y & 0x01;

  if(!richtung){
    gerade = !gerade;
  }
  
  if(gerade) {
    // Odd rows run backwards
    i = tLEDs(y) - x;
    } 
  else {
      // Even rows run forwards
      if (y == 0){
        i = x;
      }
      else{
        i = tLEDs(y) - currentLEDs(y) + x + 1;
      }
  }
  return i;
}

uint16_t xySky( uint8_t x, uint8_t y){
  uint16_t i;
  
  if(x < stepsSky[y] && x < currentLEDs(y)){
    if( y & 0x01) {
      // Odd rows run backwards
      i = tLEDs(y) - x;
    }
    else {
      // Even rows run forwards
      if (y == 0){
        i = x;
      }
      else{
        i = tLEDs(y) - currentLEDs(y) + x + 1;
      }
    }
  }
  else {
    i = 9999;
  }
  return i;
}

uint16_t xyBerg( uint8_t x, uint8_t y){
  uint16_t i;
  
  if(x < stepsSky[y]){
      i = 9999;  
  }
  else {
    if( y & 0x01) {
      // Odd rows run backwards
      i = tLEDs(y) - x;
    }
    else {
      // Even rows run forwards
      if (y == 0){
        i = x;
      }
      else{
        i = tLEDs(y) - currentLEDs(y) + x + 1;
      }
    }
  }
  return i;
}
