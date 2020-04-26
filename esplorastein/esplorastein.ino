#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>            // Arduino LCD library

// X IS FOR LINES
// Y IS FOR COLUMNSi
// 0 0 IS TOP LEFT
#define MAXX 32
#define MAXY 40
class Map
{
public:

    inline uint16_t offset(uint8_t x, uint8_t y)
    {
      return x*MAXY + y;
    }

    void set(uint8_t x, uint8_t y)
    {
        data[offset(x,y)] = 1;
    }

    void unset(uint8_t x, uint8_t y)
    {
        data[offset(x,y)] = 0;
    }

    // 0 if empty, nonzero if full
    uint8_t at(uint8_t x, uint8_t y)
    {
        return data[offset(x,y)];
    }

    void clear()
    {
        for(uint16_t i = 0; i < (MAXX*(MAXY)); ++i)
        {
            data[i] = 0;
        }
    }

    void randomize(uint8_t threshold)
    {
        for(uint16_t i = 0; i < (MAXX*(MAXY)); ++i)
        {
            data[i] = (uint8_t)(random(256) < threshold);
        }
    }

    void tftPrint()
    {
        EsploraTFT.background(0,0,0);
        EsploraTFT.fill(0,0,0);
        for(uint8_t x = 0; x < MAXX; ++x)
        {
            for(uint8_t y = 0; y < MAXY; ++y)
            {
                uint8_t val = at(x,y);
                Serial.println(val);
                val = (val != 0 ? 255 : 0);
                EsploraTFT.stroke(val,val,val);
                EsploraTFT.rect(y<<2,x<<2,4,4);
            }
        }
    }
private:
    uint8_t data[MAXX*MAXY] = {0};
};

class Player
{
public:
    void tftPrint()
    {
        EsploraTFT.stroke(255,0,0);
        EsploraTFT.fill(255,0,0);
        EsploraTFT.rect(y<<2,x<<2,4,4);
        //EsploraTFT.fill(0,0,255);
        /*
        switch(angle)
        {
            case(0):
            case(360):
                 EsploraTFT.rect(y<<2,x<<2,4,4);
        }
        
        EsploraTFT.rect(y<<2,x<<2,4,4);*/
    }

    void ray(uint16_t angle)
    {
      
    }

private:
    uint8_t x = 0, y = 0;
    uint16_t angle = 0;
};

Player p;
Map m;
void setup() 
{
    Serial.begin(115200);
    EsploraTFT.begin();
    randomSeed(analogRead(0));
    EsploraTFT.background(0,0,0);
    m.randomize(50);
    m.tftPrint();
    delay(1000);
    m.clear();
    m.tftPrint();
    delay(1000);
    m.randomize(50);
    m.tftPrint();
    p.tftPrint();
}

void loop()
{
}
