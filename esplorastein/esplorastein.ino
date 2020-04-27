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
#define ANG90 1
#define ANG180 2
#define ANG270 3

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
    Player(uint8_t ax, uint8_t ay, uint8_t aang, Map * am)
    {
        x = ax;
        lastx = ax;
        y = ay;
        lasty = ay;
        angle = aang & 3;
        m = am;
    }
  
    void print()
    {
        Serial.println("PLAYER: ");
        Serial.print("X\t");
        Serial.println(x);
        Serial.print("Y\t");
        Serial.println(y);
        Serial.print("A\t");
        Serial.println(angle);
    }

    void tftPrint()
    {
        EsploraTFT.stroke(0,0,0);
        EsploraTFT.fill(0,0,0);
        EsploraTFT.rect(lasty<<2,lastx<<2,4,4);
      
        EsploraTFT.stroke(255,0,0);
        EsploraTFT.fill(255,0,0);
        EsploraTFT.rect(y<<2,x<<2,4,4);

        lastx = x;
        lasty = y;

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

    inline void right()
    {
        angle = (angle + 1) & 3;
    }

    inline void left()
    {
         angle = (angle - 1) & 3;
    }

    inline bool forward()
    {
        uint8_t newx = x;
        uint8_t newy = y;
        switch(angle)
        {
            case ANG0:    newx = x-1; break;
            case ANG90:   newy = y+1; break;
            case ANG180:  newx = x+1; break;
            case ANG270:  newy = y-1; break;
            default: Serial.println("Forwards bad angle");
        }
        if(newx >= MAXX || newy >= MAXY)
            return false;

        if(m->at(newx,newy))
          return false;

        x = newx;
        y = newy;
        return true;
    }
    
    inline bool backward()
    {
        uint8_t newx = x;
        uint8_t newy = y;
        switch(angle)
        {
            case ANG0:    newx = x-1; break;
            case ANG90:   newy = y-1; break;
            case ANG180:  newx = x-1; break;
            case ANG270:  newy = y+1; break;
            default: Serial.println("Forwards bad angle");
        }
        if(newx >= MAXX || newy >= MAXY)
            return false;

        if(m->at(newx,newy))
          return false;

        x = newx;
        y = newy;
        return true;

        
    }
    uint8_t lastx = 0, lasty = 0;
    
    uint8_t x = 0, y = 0;
    uint8_t angle = 0;
    Map * m;
};


constexpr uint16_t button_conf_thr = 20;
uint16_t button_conf[4] = {0};
void sample_buttons()
{
    for( int i = 1; i < 5; ++i )
    {
        if(Esplora.readButton(i) == LOW)
        {
            if(button_conf[i-1] < button_conf_thr)
              button_conf[i-1]++;
        }
        else
        {
            button_conf[i-1]=0;
        }
    }
}

int button_press()
{
    static bool last[4] = {false};
    
    int r = 0;
    for( int i = 1; i < 5; ++i )
    {
        bool button = button_conf[i-1] >= button_conf_thr;
        if(button && !last[i-1])
        {
            r = i;
            last[i-1] = button;
            break;
        }
        last[i-1] = button;
    }
    return r;
}





Map m;
Player p(10,10,0, &m);
void setup() 
{
    Serial.begin(115200);
    EsploraTFT.begin();
    randomSeed(analogRead(0));
    EsploraTFT.background(0,0,0);
    m.clear();
    m.randomize(20);
    m.tftPrint();
    p.tftPrint();
}

void loop()
{
    sample_buttons();
    switch(button_press())
    {
        case 0:
          /*Serial.println("-----");
          for(int i = 0; i < 4; ++i)
            Serial.println(button_conf[i]);
          Serial.println("-----");*/
          break;
        case SWITCH_UP:
          Serial.println("UP");
          p.forward();
          p.print();
          p.tftPrint();
          break;
        case SWITCH_DOWN:
          Serial.println("DOWN");
          p.backward();
          p.print();
          p.tftPrint();
          break;
        case SWITCH_LEFT:
          Serial.println("LEFT");
          p.left();
          p.print();
          p.tftPrint();
          break;
        case SWITCH_RIGHT:
          Serial.println("RIGHT");
          p.right();
          p.print();
          p.tftPrint();
          break;
        default:
          Serial.println("Bordel");
    }
}
