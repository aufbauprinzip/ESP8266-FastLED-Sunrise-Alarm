void HintermBerg(){
  for(int r = 0; r < 255; r++){
    for (int y = kMatrixStart; y < kMatrixHeight; y++){
      for (int x = 0; x < currentLEDs(y); x++){
        float grad = float(x) / float(stepsSky[y]);
        float k2 = (float(y) / float(kMatrixHeight)) * float(r);

        //Basic version; Problem: lower rows still show red
        //float k = float(y) / float(kMatrixHeight) * float(r);
        
        //Speeds up color gradient
        //float k = sqrt(float(y) / float(kMatrixHeight)) * float(r);

        //Improves color gradient, so that even bottom rows show blue
        float v = float(y) / float(kMatrixHeight);
        float k = pow(v, v) * float(r);  
        
        //float sinus = sin8(k - float(64.75));
        float sinus = sin8(k2 - float(64.75));
        float j = float(255) - k;
        int grad2 = grad * j * (sinus / 255) + k;
        leds[xySky(stepsSky[y] -  x, y)] =  ColorFromPalette(myPal, grad2); //
        leds[xyBerg(x, y)] =  CHSV(227, 20, r);
      }
    }
    FastLED.setBrightness(r);
    FastLED.show();
    delay(10);
  }
}

//Needs more work
void sternenschauer(){
  EVERY_N_SECONDS(random8(10)){
    int rand_y = random8(4, 15);
    int StarBright = 255;
    for (int x = 0; x < currentLEDs(rand_y); x++){
      leds[xySky(x,rand_y)] = CHSV(0, 0, 255);
      FastLED.show();
      delay(10);
      StarBright -= 10;
      fadeToBlackBy(leds, NUM_LEDS, 50);
    }
    memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
  }
}


//Welle von Farben von links unten nach rechts oben
void LilaWolken(){
  for (int y = kMatrixStart; y < kMatrixHeight; y++){
    for (int x = 0; x < currentLEDs(y); x++){
      leds[xySky(x, y)] = CHSV(gHue, 255, 255);
    }
    FastLED.show();
    fadeToBlackBy(leds, NUM_LEDS, 20);
    gHue+=16;
    delay(50);
  }
}

void SkyWaveBergSolid(){
  for (int y = kMatrixStart; y < kMatrixHeight; y++){
    for (int x = 0; x < currentLEDs(y); x++){
      leds[xySky(x, y)] = CHSV(gHue, 255, 80);
    }
    FastLED.show();
    fadeToBlackBy(leds, NUM_LEDS, 50);
  for (int y = kMatrixStart; y < kMatrixHeight; y++){
    for (int x = 0; x < currentLEDs(y); x++){
        leds[xyBerg(x, y)] = CHSV(gHue, 0, 80);
    }
  }
  gHue+=16;
  delay(50);
  }
}

void Horizont(){
  for(int r = 0; r < 255; r++){
    for (int y = kMatrixStart; y < kMatrixHeight; y++){
      for (int x = 0; x < currentLEDs(y); x++){
        float grad = float(x) / float(currentLEDs(y));
        float k2 = (float(y) / float(kMatrixHeight)) * float(r);

        //Basic version; Problem: lower rows still show red
        float k = float(y) / float(kMatrixHeight) * float(r);
        
        //Speeds up color gradient
        //float k = sqrt(float(y) / float(kMatrixHeight)) * float(r);

        //Improves color gradient, so that even bottom rows show blue
        //float v = float(y) / float(kMatrixHeight);
        //float k = pow(v, v) * float(r);  
        
        float sinus = sin8(k2 - float(64.75));
        float j = float(255) - k;
        int grad2 = grad * j * (sinus / 255) + k;
        leds[xyAll(x, y, false)] =  ColorFromPalette(myPal, grad2);
      }
    }
    FastLED.show();
    delay(10);
  }
}

void HorizontStatischSimple(){
  for (int y = kMatrixStart; y < kMatrixHeight; y++){
    for (int x = 0; x < currentLEDs(y); x++){
      float grad = float(x) / float(currentLEDs(y));
      int grad2 = grad * 255;
      leds[xyAll(x, y, false)] =  ColorFromPalette(myPal, grad2);
    }
  }
  FastLED.show();
}

void HorizontStatischSinus(){
  for (int y = kMatrixStart; y < kMatrixHeight; y++){
    for (int x = 0; x < currentLEDs(y); x++){
      float grad = float(x) / float(currentLEDs(y));
      float k = (float(y) / float(kMatrixHeight)) * 255; 
      float sinus = sin8(k - float(64.75));
      int grad2 = grad * (sinus / 255) * 255;
      leds[xyAll(x, y, false)] =  ColorFromPalette(myPal, grad2);
    }
  }
  FastLED.show();
}

void HorizontStatisch(){
  for (int y = kMatrixStart; y < kMatrixHeight; y++){
    for (int x = 0; x < currentLEDs(y); x++){
      float grad = float(x) / float(currentLEDs(y));
      float k = float(y) / float(kMatrixHeight) * 255;  
      float sinus = sin8(k - float(64.75));
      float j = float(255) - k;
      int grad2 = grad * (sinus / 255)* j + k ;
      leds[xyAll(x, y, false)] =  ColorFromPalette(myPal, grad2);
    }
  }
    FastLED.show();
}

void HorizontSinXSinY(){
  //for(int r = 0; r < 255; r++){
  for (int y = kMatrixStart; y < kMatrixHeight; y++){
    for (int x = 0; x < currentLEDs(y); x++){
      float grad = float(x) / float(currentLEDs(y)) * 255;
      float k = float(y) / float(kMatrixHeight) * 255;  
      float sinusy = sin8(k - float(64.75));
      float sinusx = sin8(grad - float(64.75));
      float j = float(255) - k;
      int grad2 = (sinusx / 255) * (sinusy / 255) * 255;
      leds[xyAll(x, y, false)] =  ColorFromPalette(myPal, grad2);
    }
  }
    FastLED.show();
  //}
}

/* Gradient von rechts oben nach unten nach link unten wenn w = 0
 * Oder von links oben nach rechts unten wenn w = 255
 */
void HintermBergSchraeg(){
  int w = 255;
  for(int r = 0; r < 255; r++){
    for (int y = kMatrixStart; y < kMatrixHeight; y++){
      for (int x = 0; x < stepsSky[y]; x++){
        float grad = float(x) / float(stepsSky[y]);
        float k2 = (float(y) / float(kMatrixHeight)) * float(w);
        float k = sqrt(float(y) / float(kMatrixHeight)) * float(r); 
        float sinus = sin8(k2 - float(64.75));
        float j = float(255) - k;
        int grad2 = grad * j * (sinus / 255) + k;
        leds[xySky(stepsSky[y] - x, y)] =  ColorFromPalette(myPal, grad2);
      }
    }
    FastLED.show();
  }
}
