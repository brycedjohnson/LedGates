FASTLED_USING_NAMESPACE

#define FASTLED_ESP32_I2S

#include "FastLED.h"
#include "wifigates.h"
#include "blegates.h"
#include "persist.h"
#include "tft.h"

#define STRIP1_PIN    12
#define STRIP2_PIN    26
#define STRIP3_PIN    25
#define STRIP4_PIN    33
#define STRIP5_PIN    13
#define STRIP6_PIN    15
#define STRIP7_PIN    22
#define STRIP8_PIN    21

//#define CLK_PIN   4
#define LED_TYPE    WS2813
#define COLOR_ORDER GRB
#define NUM_LEDS_PER_STRIP 354 //50 //354
#define NUM_STRIPS 1
#define NUM_LEDS    (NUM_LEDS_PER_STRIP * NUM_STRIPS)
CRGB leds[NUM_LEDS];

#define OFFSET NUM_LEDS_PER_STRIP

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define BRIGHTNESS_DEFAULT 96
#define TYPE_DEFAULT 0
#define SPEED_DEFAULT 100
#define SKIP_DEFAULT 0
#define OFFSET_DEFAULT 9

static int patternType = TYPE_DEFAULT;
static int patternSpeed = SPEED_DEFAULT;
static int patternSkip = SKIP_DEFAULT;
static int patternOffset = OFFSET_DEFAULT;

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");
  tftInit();
  persistInit();
  bleInit();
  wifiInit();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,STRIP1_PIN,COLOR_ORDER>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,STRIP2_PIN,COLOR_ORDER>(leds, OFFSET * 1, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,STRIP3_PIN,COLOR_ORDER>(leds, OFFSET * 2, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,STRIP4_PIN,COLOR_ORDER>(leds, OFFSET * 3, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,STRIP5_PIN,COLOR_ORDER>(leds, OFFSET * 4, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,STRIP6_PIN,COLOR_ORDER>(leds, OFFSET * 5, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,STRIP7_PIN,COLOR_ORDER>(leds, OFFSET * 6, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,STRIP8_PIN,COLOR_ORDER>(leds, OFFSET * 7, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(getBrightness());
  patternType = getType();
  patternSpeed = getSpeed();
  patternOffset = getOffset();

  Serial.println("Init complete");
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { sprial, sprial2, sinelon, Fire2012, confetti, rainbow, bpm, redtest, bluetest, greentest};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
    EVERY_N_SECONDS(1) {
      wifiLoopHandle();
      tftLoop();
    }

    for (int i = 0; i <= patternSkip; i++) {
      // Call the current pattern function once, updating the 'leds' array
      gPatterns[gCurrentPatternNumber]();
    }

    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    if (patternSpeed < 600) {
      FastLED.delay(1000/patternSpeed); 
    }

    // do some periodic updates
    EVERY_N_MILLISECONDS(20) { 
      gHue++; 
    } // slowly cycle the "base color" through the rainbow
  
    if (patternType < ARRAY_SIZE( gPatterns)) {
        gCurrentPatternNumber = patternType;
    } else {
        //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
    }
}

void setBrightness(int32_t bright)
{
  if (bright < 255) {
    persistSet_i32("brightness", bright);
    FastLED.setBrightness(bright);
  }
}

int32_t getBrightness(void)
{
  int32_t value = persistGet_i32("brightness");
  if (value < 0) {
    value = BRIGHTNESS_DEFAULT;
  }
  return value;
}

void setType(int32_t type)
{
  if (type < ARRAY_SIZE( gPatterns)) {
    persistSet_i32("type", type);
    patternType = type;
  }
}

int32_t getType(void)
{
  int32_t value = persistGet_i32("type");
  if (value < 0) {
    value = TYPE_DEFAULT;
  }
  return value;
}

void setSpeed(int32_t speed)
{
    persistSet_i32("speed", speed);
    patternSpeed = speed;
}

int32_t getSpeed(void)
{
  int32_t value = persistGet_i32("speed");
  if (value < 0) {
    value = SPEED_DEFAULT;
  }
  return value;
}

void setSkip(int32_t skip)
{
    persistSet_i32("skip", skip);
    patternSkip = skip;
}

int32_t getSkip(void)
{
  int32_t value = persistGet_i32("skip");
  if (value <= 0) {
    value = SKIP_DEFAULT;
  }
  return value;
}

void setOffset(int32_t offset)
{
  if (patternOffset <= 0) {
      patternOffset = 1;
  } else {
      patternOffset = offset;
  }
  persistSet_i32("offset", offset);

}

int32_t getOffset(void)
{
  int32_t value = persistGet_i32("offset");
  if (value < 0) {
    value = OFFSET_DEFAULT;
  }
  return value;
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  gHue++;
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 1);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

#define NUM_SINELON 2
void sinelon()
{
  uint8_t hue;
  static int lastpos[NUM_SINELON];
  int pos[NUM_SINELON];
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 1);
  for (int i=0; i < NUM_SINELON; i++) {
    hue = gHue+64*i;
    pos[i] = beatsin16( 10, 0, NUM_LEDS-1,0,(UINT16_MAX*i)/NUM_SINELON);

    if (pos[i] > lastpos[i]) {
        for (; lastpos[i] <= pos[i]; lastpos[i]++) {
            leds[lastpos[i]] += CHSV( hue, 255, 192);
        } 
    } else {
        for (; lastpos[i] >= pos[i]; lastpos[i]--) {
            leds[lastpos[i]] += CHSV( hue, 255, 192);
        }    
    }
  }
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 122;
  CRGBPalette16 palette = PartyColors_p;
  for (int j = 0 ; j < NUM_STRIPS; j++) {
    uint8_t beat = beatsin8( BeatsPerMinute, 130, 255,0,j*10*patternOffset);
    for( int i = 0; i < NUM_LEDS_PER_STRIP; i++) { //9948
        leds[i+j*NUM_LEDS_PER_STRIP] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    }
  } 
}

void juggle()
{
  // 4 colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 5);
  byte dothue = 0;
  for( int i = 0; i < 4; i++) {
    leds[beatsin16( i+8, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

//#define SPRIAL_OFFSET 9
void sprial()
{
  static uint32_t pos = 0;
  fadeToBlackBy( leds, NUM_LEDS, 10);
  byte dothue = 0;//gHue;
  pos = pos + 1;
  if (pos >= NUM_LEDS) {
      pos = 0;
      gHue++;
  }
  for (int i = 0; i < patternOffset; i++) {
    dothue = gHue;
    leds[(pos + ((NUM_LEDS * i) / patternOffset)) % NUM_LEDS] = CRGB (255,255,255);
    leds[(pos + ((NUM_LEDS * i) / patternOffset) - 1) % NUM_LEDS] = CRGB (255,255,255);
    leds[(pos + ((NUM_LEDS * i) / patternOffset) - 2) % NUM_LEDS] = CHSV(dothue, 200, 255);
  }
}

void sprial2()
{
  static uint32_t pos = 0;
  fadeToBlackBy( leds, NUM_LEDS, 10);
  static byte dothue = 0;//gHue;

  pos = pos + 1;
  if (pos >= NUM_LEDS) {
      pos = 0;
      dothue++;
  }
  for (int i = 0; i < patternOffset; i++) {
    leds[(pos + (NUM_LEDS * i / patternOffset)) % NUM_LEDS] = CRGB (255,255,255);
    leds[(pos + (NUM_LEDS * i / patternOffset) - 1) % NUM_LEDS] = CRGB (255,255,255);
    leds[(pos + (NUM_LEDS * i / patternOffset) - 2) % NUM_LEDS] = CHSV(dothue, 200, 255);
  }
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  85

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 140


#define FIRE_LEDS (NUM_LEDS_PER_STRIP/2)
#define FIRE_SEQ_STRIPS (NUM_STRIPS*2)
void Fire2012()
{
// Array of temperature readings at each simulation cell
    static byte heat[FIRE_SEQ_STRIPS][FIRE_LEDS];
    bool gReverseDirection = false;

    for (int strip = 0; strip< FIRE_SEQ_STRIPS; strip++) {
    // Step 1.  Cool down every cell a little
        for( int i = 0; i < FIRE_LEDS; i++) {
            heat[strip][i] = qsub8( heat[strip][i],  random8(0, ((COOLING * 10) / FIRE_LEDS) + 2));
        }
    
        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for( int k= FIRE_LEDS - 1; k >= 2; k--) {
            heat[strip][k] = (heat[strip][k - 1] + heat[strip][k - 2] + heat[strip][k - 2] ) / 3;
        }
        
        // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
        if( random8() < SPARKING ) {
            int y = random8(7);
            heat[strip][y] = qadd8( heat[strip][y], random8(160,255) );
        }

        // Step 4.  Map from heat cells to LED colors
        for( int j = 0; j < FIRE_LEDS; j++) {
            CRGB color = HeatColor( heat[strip][j]);
            int pixelnumber;
            if (gReverseDirection) {
                pixelnumber = (FIRE_LEDS-1) - j;
            } else {
                pixelnumber = j;
            }
            leds[strip*FIRE_LEDS + pixelnumber] = color;            

        }
        if (gReverseDirection) {
            gReverseDirection = false;
        } else {
            gReverseDirection = true;
        }
    }
}

void redtest() 
{
  static uint32_t i =0;
  i++;
  leds[i % NUM_LEDS] = CRGB (255,255,255);
  leds[(i - 1) % NUM_LEDS] = CRGB (255,255,255);
  leds[(i - 2) % NUM_LEDS] = CHSV(HUE_RED, 200, 255);
}

void greentest() 
{
  static uint32_t i = 0;
  i++;
  leds[i % NUM_LEDS] = CRGB (255,255,255);
  leds[(i - 1) % NUM_LEDS] = CRGB (255,255,255);
  leds[(i - 2) % NUM_LEDS] = CHSV(HUE_GREEN, 200, 255);
}

void bluetest() 
{
  static uint32_t i = 0;
  i++;
  leds[i % NUM_LEDS] = CRGB (255,255,255);
  leds[(i - 1) % NUM_LEDS] = CRGB (255,255,255);
  leds[(i - 2) % NUM_LEDS] = CHSV(HUE_BLUE, 200, 255);
}