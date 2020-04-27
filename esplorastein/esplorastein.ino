#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>            // Arduino LCD library

// X IS FOR LINES
// Y IS FOR COLUMNSi
// 0 0 IS TOP LEFT
//Aangles represented as 16bit int

#define MAXX 32
#define MAXY 40

#define ANG0 0
#define ANG90 16384
#define ANG180 32687
#define ANG270 49152
#define ANG360 0

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

uint16_t toMyAngle(uint16_t degs)
{
    switch(degs)
    {
        case 0:        return ANG0;
        case 90:       return ANG90;
        case 180:      return ANG180;
        case 270:      return ANG270;
        case 360:      return ANG360;
        default:       return degs * 182;
    }
}

uint16_t fromMyAngle(uint16_t ang)
{
    switch(ang)
    {
        case ANG0:     return 0;
        case ANG90:    return 90;
        case ANG180:   return 180;
        case ANG270:   return 270;
        default:       return ang / 182;
    }
}

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

    void ray(uint16_t ang)
    {
        uint16_t absangle = angle + ang;
    }

    inline void right(uint16_t degs)
    {
        angle += toMyAngle(degs);
    }

    inline void left(uint16_t degs)
    {
        angle -= toMyAngle(degs);
    }

    inline void forward()
    {
        switch(angle)
        {
            case ANG0: x--;
            case ANG90: y++;
            case ANG180: x++;
            case ANG270: y--;
            default: Serial.println("Bad angle");
        }
    }
    
    inline void backward()
    {
        switch(angle)
        {
            case ANG0: x++;
            case ANG90: y--;
            case ANG180: x--;
            case ANG270: y++;
            default: Serial.println("Bad angle");
        }
    }

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
    m.clear();
    m.tftPrint();
    p.x = 10;
    p.y = 10;
    p.tftPrint();
}

void loop()
{
    Serial.println("kek");
    p.tftPrint();
    p.forward();
    delay(1000);
}
