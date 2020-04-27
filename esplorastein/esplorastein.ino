#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>            // Arduino LCD library

// X IS FOR LINES
// Y IS FOR COLUMNSi
// 0 0 IS TOP LEFT
//Aangles represented as 16bit int

#define MAXX 32
#define MAXY 40
#define SCALE 4
#define SCALESHIFT 2

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

    uint8_t ray(uint8_t stepping, bool left)
    {
        uint8_t lx, ly, cnt;
        uint8_t distance = 0;
        lx = (x << SCALESHIFT) + 2;
        ly = (y << SCALESHIFT) + 2;
        cnt = 0;
        EsploraTFT.stroke(0,0,255);
        while(true)
        {
            EsploraTFT.point(ly,lx);
            if(lx > (MAXX*SCALE))
            {
                Serial.println("LX");
                return distance;
            }
            if(ly > (MAXY*SCALE))
            {
                Serial.println("LY");
                return distance;
            }
            if(m->at(lx >> SCALESHIFT, ly >> SCALESHIFT) != 0)
            {
                Serial.println("COLLISION");
                return distance;
            }

            if(cnt >= stepping)
            {
                Serial.println(cnt);
                switch(angle)
                {
                    case 0:
                        if(left)
                            --ly;
                        else
                            ++ly;
                        break;
                    case 1:
                        if(left)
                            --lx;
                        else
                            ++lx;
                        break;
                    case 2:
                        if(left)
                            ++ly;
                        else
                            --ly;
                        break;
                    case 3:
                        if(left)
                            ++lx;
                        else
                            --lx;
                        break;
                    default:
                        Serial.println("raybadangle");
                }
                cnt = 255;
            }
            else
            {
                switch(angle)
                {
                    case 0:
                        --lx;
                        break;
                    case 1:
                        ++ly;
                        break;
                    case 2:
                        ++lx;
                        break;
                    case 3:
                        --ly;
                        break;
                    default:
                        Serial.println("raybadangle");
                }
            }
            ++cnt;
            ++distance;
        }
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
            case ANG0:    newx = x+1; break;
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

void cls()
{
    Serial.write(27);       // ESC command
    Serial.print("[2J");    // clear screen command
    Serial.write(27);
    Serial.print("[H");     // cursor to home command
}

void gameprint(Map * m, Player * p)
{
    cls();
    for(uint8_t i = 0; i < MAXY+2; ++i)
    {
        if(i == 0 || i == MAXY+2-1)
            Serial.print('O');
        else
            Serial.print('-');
    }
    Serial.println();
    for(uint8_t x = 0; x < MAXX; ++x)
    {
        Serial.print('|');
        for(uint8_t y = 0; y < MAXY; ++y)
        {
            uint8_t val = m->at(x,y);
            if(val)
                Serial.print('@');
            else if(p->x == x && p->y == y)
            switch(p->angle)
            {
                case 0:
                    Serial.print('A');
                    break;
                case 1:
                    Serial.print('>');
                    break;
                case 2:
                    Serial.print('V');
                    break;
                case 3:
                    Serial.print('<');
                    break;
            }
            else
                Serial.print(' ');
        }
        Serial.print('|');
        Serial.println();
    }
    for(uint8_t i = 0; i < MAXY+2; ++i)
    {
        if(i == 0 || i == MAXY+2-1)
            Serial.print('O');
        else
            Serial.print('-');
    }
    Serial.println();
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
          //Serial.println("UP");
          p.forward();
          gameprint(&m,&p);
          p.tftPrint();
          break;
        case SWITCH_DOWN:
          //Serial.println("DOWN");
          p.backward();
          gameprint(&m,&p);
          p.tftPrint();
          p.ray(255,false);
          p.ray(1,false);
          p.ray(2,false);
          p.ray(3,false);
          p.ray(1,true);
          p.ray(2,true);
          p.ray(3,true);
          break;
        case SWITCH_LEFT:
          //Serial.println("LEFT");
          p.left();
          gameprint(&m,&p);
          p.tftPrint();
          break;
        case SWITCH_RIGHT:
          //Serial.println("RIGHT");
          p.right();
          gameprint(&m,&p);
          p.tftPrint();
          break;
        default:
          Serial.println("Bordel");
    }
}
