#include <Arduino.h>


// -----tft-------------
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

#define TFT_SCLK 13
#define TFT_MOSI 11 //brukade vara på pin 11..men ska nu ha den för musik...

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
  //------------

//--------- Buttons A1 ------------
#include <OnewireKeypad.h>

#define Rows 4
#define Cols 4
#define Pin A1
#define Row_Res 4700
#define Col_Res 1000
// ExtremePrec, HighPrec, MediumPrec, LowPrec
#define Precision LowPrec

char KEYS[]= {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};

OnewireKeypad <Print, 16> KP(Serial, KEYS, Rows, Cols, Pin, Row_Res, Col_Res, Precision );
//----------------------------------




// ------ Neopixel -----------------
#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, SIREN_EFFECT,SIREN_EFFECT2, COLOR_WIPE, SCANNER, FADE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };


//----buttons values...
int disco_efx = 0;
int cops_efx = 0;
int headlight_efx = 0;


// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:

    // Member Variables:
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position

    uint32_t Color1, Color2,Color3;  // What colors are in use  ---LA TILL COLOR3...
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern

    void (*OnComplete)();  // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }

    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case RAINBOW_CYCLE:
                    RainbowCycleUpdate();
                    break;
                case THEATER_CHASE:
                    TheaterChaseUpdate();
                    break;
                case SIREN_EFFECT:
                    SirenEffectUpdate();
                    break;

                    case SIREN_EFFECT2:
                        SirenEffectUpdate2();
                        break;

                case COLOR_WIPE:
                    ColorWipeUpdate();
                    break;
                case SCANNER:
                    ScannerUpdate();
                    break;
                case FADE:
                    FadeUpdate();
                    break;
                default:
                    break;
            }
        }
    }

    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else // Direction == REVERSE
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
    }

    // Reverse pattern direction
    void Reverse()
    {
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            Index = 0;
        }
    }

    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;
    }

    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        show();
        Increment();
    }

    // Initialize for a Theater Chase
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
   }

    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            if ((i + Index) % 3 == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                setPixelColor(i, Color2);
            }
        }
        show();
        Increment();
    }

    // Initialize for a Siren effect

    // skulle gå att använda som blinkers oxo!!! om man skriver OM funktionen..så det ta in en till parameter..Vänster höger blink..typ..

//typ

  //  void SirenEffect(uint32_t color1,uint32_t color2, uint8_t interval, int blinkers  -> 0 INGEN 1 VÄNSTER 2 HÖGER..TYP..

    void SirenEffect(uint32_t color1,uint32_t color2, uint8_t interval)
    {
        ActivePattern = SIREN_EFFECT;
        Interval = interval;
        TotalSteps = 12; //hur många ggr vill man köra denna effet...
        Color1 = color1;
        Color2 = color2;
        Index = 0;
   }

    // Update the Theater Chase Pattern
    void SirenEffectUpdate()
    {
     uint32_t black = Color(0, 0, 0);

      if (Index % 2 == 0)
      {
        int off= 15;
        for (int on=0; on < 15; on++)
        {
         setPixelColor(on, Color1); // RED
         setPixelColor(off, black);
         off++;
        }
        show();
      }
      else
      {
        int off=0;

        for (int on=15; on<30; on++)
        {
             setPixelColor(on, Color2); // blue...
             setPixelColor(off, black);
             off++;
        }
        show();
      }

      Increment();
    }

    void SirenEffect2(uint32_t color1,uint32_t color2, uint8_t interval)
    {
        ActivePattern = SIREN_EFFECT;
        Interval = interval;
        TotalSteps = 12; //hur många ggr vill man köra denna effet...
        Color1 = color1;
        Color2 = color2;
        Index = 0;
   }

    // Update the Theater Chase Pattern
    void SirenEffectUpdate2()
    {
     uint32_t black = Color(0, 0, 0);

      if (Index % 2 == 0)
      {
        int off= 15;
        for (int on=0; on < 15; on++)
        {
         setPixelColor(on, Color1); // RED
         setPixelColor(off, black);
         off++;
        }
        show();
      }
      else
      {
        int off=0;

        for (int on=15; on<30; on++)
        {
             setPixelColor(on, Color2); // blue...
             setPixelColor(off, black);
             off++;
        }
        show();
      }

      Increment();
    }






    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color;
        Index = 0;
        Direction = dir;
    }

    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
        setPixelColor(Index, Color1);
        show();
        Increment();
    }

    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 0;
    }

    // Update the Scanner Pattern
    void ScannerUpdate()
    {

        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
            }
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
    }

    // Initialize for a Fade
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }

    // Update the Fade Pattern
    void FadeUpdate()
    {
        // Calculate linear interpolation between Color1 and Color2
        // Optimise order of operations to minimize truncation error
        uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
        uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
        uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

        ColorSet(Color(red, green, blue));
        show();
        Increment();
    }

    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
        // Shift R, G and B components one bit to the right
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }

    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }
};

//void Ring1Complete();
//void Ring2Complete();
void StickComplete();

// Define some NeoPatterns for the two rings and the stick
//  as well as some completion routines
//NeoPatterns Ring1(24, 5, NEO_GRB + NEO_KHZ800, &Ring1Complete);
//NeoPatterns Ring2(16, 6, NEO_GRB + NEO_KHZ800, &Ring2Complete);
NeoPatterns Stick(60, 6, NEO_GRB + NEO_KHZ800, &StickComplete);

//-- buttons functions..

// disco kulan .. alla effekter efter varandra..
void doit1()
{
  disco_efx++;

  if (disco_efx == 1)
  {
    Stick.Scanner(Stick.Wheel(random(255)), 55);
  }
  if (disco_efx == 2)
  {
    Stick.ActivePattern = RAINBOW_CYCLE;
    Stick.TotalSteps = 255;
    Stick.Interval = min(10, Stick.Interval);
  }
  if (disco_efx == 3)
  {
    Stick.Color1 = Stick.Wheel(random(255));
    Stick.Color2 = Stick.Wheel(random(255));
    Stick.Color3 = Stick.Wheel(random(255));

    Stick.TheaterChase(Stick.Color(Stick.Color1,Stick.Color2,Stick.Color3), Stick.Color(Stick.Color1,Stick.Color1,Stick.Color1), 100);

    disco_efx = 0; // goto 10..back to first effect..
  }
}

void doit2()
{

}
void doit3()
{


}
// turn LEDS OFF
void doitA()
{
  Stick.ColorWipe(Stick.Color(0,0,0), 0);
// funkar inte så att säga-> fryser alla leds då..  Stick.ActivePattern = NONE; // vad gör den...? behövs?
}
// police effect..several effects combined..
void doit4()
{

//https://www.tindie.com/products/PhoenixCNC/neopixel-police-light-pcb-with-atmega328/
   cops_efx++;
   Stick.SirenEffect(Stick.Color(255,0,0),Stick.Color(0,0,255), 1000);

}
// Head Light ..in tree leves.. full medium light...4 now.
void doit5()
{
  headlight_efx++;

  if (headlight_efx == 1)
  {
     Stick.ColorWipe(Stick.Color(255,255,255), 0);
  }
  if (headlight_efx == 2)
  {
     Stick.ColorWipe(Stick.Color(100,100,100), 0);
  }
  if (headlight_efx == 3)
  {
    Stick.ColorWipe(Stick.Color(50,50,50), 0);
    headlight_efx = 0; // goto 10..back to first effect..
  }
  //KANSKE SÄTTA FÄRG ... HELLJUSMED FÄRG....why not...
}
// blinka HÖGER
void doit6()
{
     Stick.SirenEffect(Stick.Color(255,64,0),Stick.Color(0,0,0), 1000);
}
// blinka VÄNSTER
void doitB()
{
   Stick.SirenEffect(Stick.Color(0,0,0),Stick.Color(255,64,0), 1000);
}

// Stick Completion Callback
void StickComplete()
{

    Serial.println("stick complete called...."); // detta kallas när ljusgrejjen är klar..


    //  if cops -> run...
    if (cops_efx == 1)
    {
         Serial.println("cops = 1 ..red scan!");
         Stick.Scanner(Stick.Color(255, 0, 0), 7); // red....

         cops_efx++;
    }
    else if (cops_efx == 2)
    {
         Serial.println("cops = 2 ..blue scan!");

         Stick.Scanner(Stick.Color(0, 0, 255), 7); // blue....

         cops_efx++;
    }
    else if (cops_efx == 3)
    {
         Serial.println("colorwipe..red");
         Stick.ColorWipe(Stick.Color(255, 0, 0), 20);

         cops_efx++;
    }
    else if (cops_efx == 4)
    {
      Serial.println("colorwipe..REVERSE blue");
    //  Stick.Reverse();
      Stick.ColorWipe(Stick.Color(0, 0, 255), 20);

         cops_efx++;
    }
    // sista effekten bör vara samma som första....så de blir en slinga...
    else if (cops_efx == 5)
    {
      Serial.println("first cop efx again...");
      Stick.SirenEffect(Stick.Color(255,0,0),Stick.Color(0,0,255), 1000);

      cops_efx = 1; // goto first efx again...suckkkk
    }
}





void setup(void) {
    Serial.begin(9600);


  tft.initR(INITR_BLACKTAB);  // You will need to do this in every sketch
  tft.fillScreen(ST7735_BLACK);


  //tft print function!
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0);
  tft.setCursor(30,0);
  tft.println("IntelliCart (C) ");

  tft.setCursor(10,20);
  tft.println("1) Leds RAINBOW");

  tft.setCursor(10,30);
  tft.println("2) Leds CHASE");

  tft.setCursor(10,40);
  tft.println("3) Leds SCANNER");

  tft.setCursor(10,50);
  tft.println("4) STOP LEDS");






    KP.SetHoldTime(1000);




 // tog denna bara..  Stick.Color1 = Stick.Wheel(random(255));
 //   Stick.Scanner(Stick.Wheel(random(255)), 55);
  Serial.println("go....");

     Stick.begin();
     Stick.show();

}

void loop() {

  char Key;
  byte KState = KP.Key_State();

  if (KState == PRESSED) {
    if ( Key = KP.Getkey() ) {
      Serial << "Pressed: " << Key << "\n";

     // stäng av tidigare slingor..
       cops_efx=0; // stäng ner all andra slingor om de nu körs..

      switch (Key) {
        case '1': doit1(); break; // disco
        case '2': doit2(); break;
        case '3': doit3(); break;
        case 'A': doitA(); break;  // led strip off
        case '4': doit4(); break;  // police
        case '5': doit5(); break;
        case '6': doit6(); break;  // blinka vänster
        case 'B': doitB(); break;  // blinka höger
        default: Serial.println("you pressed some..");
      }
    }
  }
  else if (KState == HELD) {
    Serial << "Key:" << KP.Getkey() << " being held\n";
  }


  Stick.Update();

}
