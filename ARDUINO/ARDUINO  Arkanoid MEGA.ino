#include "LedControl.h"
//game orientation TEST!
//0 == regular
//1 == paddle on the rightmost column
//2 == upside-down
//3 == paddle on the leftmost column
//change the orientation on the fly using the button
//S6 and the optional wiring described from line #223 on.
byte Orientation = 0;

//milliseconds to wait after a ball got lost
const unsigned long respawn_duration = 1500;

//the speed setting heavily depends on the used processor type
//on my Arduino Nano, speed_max = 10 and speed_min = 60 seem reasonable values.
//if you're having a faster machine, it makes sense to increase those values
const int speed_max = 10;                      //amount of screen refresh cycles that are wasted...
const int speed_min = 60;                      //...until the ball moves on one pixel
int Speed;

int Paddle = 0;                                //current x-position of paddle
byte Paddle_Old = Paddle;
byte Intensity = 8;                            //brightness of the display
byte Intensity_Old = Intensity;                

//current ball position
byte BallX = 5;                                 
byte BallY = 4;
byte BallX_Old = BallX;
byte BallY_Old = BallY;

//current ball speed in x and y direction
//note: the "char" variable can be negative, so -1 in BallDX means,
//that it moves to the left
char BallDX = 0;
char BallDY = 0;
char BallDX_tbs = 0;
char BallDY_tbs = 0;

//maximum amount of balls and current balls left
const byte Balls_max = 3;
byte Balls = 3;

//the timer is used to de-couple the paddle movement from the ball-speed
int timer = 0;

//the respawn_timer is used to wait until the next ball appears after you lose one
unsigned long respawn_timer = 0;

//used for measuring a quick paddle movement right before the ball hits the paddle
//to give the ball an extra spin; this is needed in situations, where you otherwise
//would not be able to clear all "bricks".
byte LastPaddlePos = 0;
int RelativePaddleSpeed = 0;
byte PaddleSpeedThreshold = 1;

//debounce the optional button S6 that can be used for changeing the orientation
int ButtonPressedInterval = 0;
const int ButtonPressedMinimumDuration = 10; //how long needs the button to be pressed to be recognized?

//this is the level pattern of the "bricks"
//modify to create tougher or easier levels
byte bricks[3][8] =
{
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

// :-) smiley shown, if you win the game
byte smiley_won[8][8] = 
{
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}    
};

// :-| smiley shown, if you loose the game
byte smiley_lost[8][8] = 
{
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}    
};



/* ******************************************
    MAX7221 connections to 8x8 LED matrix

    DP => ROW1        DG1 => COL1
    A  => ROW2        DG2 => COL2
    B  => ROW3        DG3 => COL3
      ...                ...    
    G  => ROW8        DG8 => COL8
    
    Additionally:
    
    1. Connect DIN, CLK, CS as shown below to digital inputs of the Arduino Nano
    2. Connect VCC to 5V and GND to GND of the Arduino Nano
*/

const byte DataIn = 2;    //D2 => DIN
const byte CLK    = 3;    //D3 => CLK
const byte LOAD   = 4;    //D4 => CS

//create a new object to control the 8x8 LED matrix
LedControl Matrix = LedControl(DataIn, CLK, LOAD, 1);

/* ****************************************** */

/* *************************************************
    1st 10k potentiometer to A0 (brightness)
    2nd 10k potentiometer to A1 (paddle)
    unused analog pin for random seed
    PWM enabled digital pin for audio
    3 digital pins for remaining ball LEDs
    
    Wiring hints:
    
    1. In the middle of your DuinoKit (Essentials)
       you find "copies" of the Arduino ports, e.g.
       A0, A1, GND and 5V. Use those "copies" to
       connect to the potentiometers as they are in
       close proximity.
       
    2. Connect A0 to the middle pin of the first
       potentiometer. It will be used for
       controlling the speed and the brightness
       
    3. Connect A1 to the middle pin of the second
       potentiometer. It will be used for playing
       the game (i.e. controlling the paddle)
       
    4. Create a "5V current column" by connecting
       5V from "the middle" with the first PIN of
       a row of the white breadboard (have a look
       at the video, minute 4:25 an onwards).
       Also create a "GND" (ground) column.
       This dramatically simplifies the further
       wiring of the potentiometers and the LEDs.
       
    5. Wire the top pin of the two potentiometers
       with your GND column on the breadboard.
       
    6. Wire the bottom pin of the two 
       potentiometers with the 5V column.
       
    7. Wire the left side of the topmost green LED
       (called LED23) with your GND column.
       
    8. Wire from here (GND LED23) directly to 
       GND LED24 and from there to GND LED 25.
       (Always on the left side, see video at the
       above-mentioned position and following.)
       
    9. Wire D5, D6 and D7 with the right side of
       the green LEDs: D5 => LED23
                       D6 => LED24
                       D7 => LED25
                       
   10. If you're using the original DuinoKit,
       wire D9 with the "+" pin of the Speaker.
       Wire the "-" pin with your GND column.
       
   11. If you're using DuinoKit Essentials, wire
       D9 with the "+" pin of the Buzzer.
       Wire the "-" pin with your GND column.
       
   12. Optional bonus wiring (tested on the original
       DuinoKit only): Wire the top left pin of
       switch S6 with D12. Wire the top right pin
       of switch S6 to GND using one of the free
       GND pins "in the middle" of your DuinoKit.
   ************************************************* */
   
const byte potBrightness = A0;
const byte potPaddle     = A1;
const byte UnusedAnalog  = A6;

const byte PWM_Audio     = 9;

const byte BallDisplay[Balls_max] = {5, 6, 7};

const byte switchOrientation =12; //D12

/* ******************************************
    SETUP
   ****************************************** */
   
void setup()
{   
    //initialize sees with floating analog number
    randomSeed(analogRead(UnusedAnalog));
        
    //The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
    Matrix.shutdown(0, false);
  
    //Set the brightness to a medium values
    Matrix.setIntensity(0, 8);

    //clear display
    Matrix.clearDisplay(0);
    
    //set the digital ports of the Arduino
    //that we use for audio output and for
    //driving the LEDs to OUTPUT
    pinMode(PWM_Audio, OUTPUT);
    for (int i = 0; i < Balls_max; i++)
        pinMode(BallDisplay[i], OUTPUT);
        
    //digital input for switch S6 (orientation)
    //activating the pullup means and wiring as described
    //above means, that the input will read HIGH when
    //the switch is open and LOW when the switch is pressed
    pinMode(switchOrientation, INPUT_PULLUP);
    
    
    //random ball start position and direction
    randomBall();
    BallDY_tbs = 1;
    respawn_timer = millis() + (2 * respawn_duration);
    
    LastPaddlePos = analogRead(potPaddle);
}

/* ******************************************
    Handle the orientation
   ****************************************** */

void handleOrientation()
{  
    if (digitalRead(switchOrientation) == LOW)
        ButtonPressedInterval++;
    else
    {
        if (ButtonPressedInterval != 0)
        {
            if (ButtonPressedInterval >= ButtonPressedMinimumDuration)
            {
                Matrix.clearDisplay(0);
                Orientation = (Orientation + 1) % 4;
            }
            
            ButtonPressedInterval = 0;
        }
    }
    
    //support "upside-down" orientations by inverting the Paddle
    if (Orientation >= 2)
        Paddle = 6 - Paddle;
}

//draws a pixel while respecting the orientation
void putPixel(byte x, byte y, boolean on)
{
    switch (Orientation)
    {
        case 0: Matrix.setLed(0, x, y, on); break;
        case 1: Matrix.setLed(0, y, 7 - x, on); break;
        case 2: Matrix.setLed(0, 7 - x, 7 - y, on); break;
        case 3: Matrix.setLed(0, 7 - y, x, on); break;
    }
}

/* ******************************************
    GAME
   ****************************************** */
   
//draw a two pixel wide paddle
void drawPaddle(byte x, boolean on)
{
    putPixel(x, 7, on);
    putPixel(x + 1, 7, on);
}

//draw the new ball and remove it at its old position
void drawBall()
{
    if ((BallX != BallX_Old) || (BallY != BallY_Old))
        putPixel(BallX_Old, BallY_Old, false);
    putPixel(BallX, BallY, true);
}

//draw a pattern, e.g. the playfield and the smileys
void drawPattern(byte pattern[8][8], byte y_count)
{
    for (byte y = 0; y < y_count; y++)
        for (byte x = 0; x < 8; x++)
            putPixel(x, y, pattern[y][x] == 1);
}

//random ball position and random x-movement
void randomBall()
{
    BallX = random(1, 7);
    BallY = random(3, 5);
    BallDX_tbs = random(10) < 5 ? 1 : -1;
    BallDY_tbs = 1;
}

//show the remaining balls in a row of LEDs
void displayBallsLeft()
{
    for (int i = 0; i < Balls_max; i++)
        digitalWrite(BallDisplay[i], i < Balls ? HIGH : LOW);
}

//play various noisy sounds
void noise(int freq1, int freq2, int duration)
{
    while (duration--)
    {
        digitalWrite(PWM_Audio, HIGH);
        delayMicroseconds(random(freq1) / 2);
        digitalWrite(PWM_Audio, LOW);
        delayMicroseconds(random(freq2) / 2);
    }
}


/* ******************************************
    MAIN LOOP
   ****************************************** */

void loop()
{    
    //sample analog data for Paddle, 8x8 display Intensity
    //and speed; only go to 1020 to avoid fibrillation
    Paddle = map(analogRead(potPaddle), 0, 1020, 0, 6);   
    int poti = analogRead(potBrightness);
    Intensity = map(poti, 0, 1020, 0, 15);
    Speed = map(poti, 0, 1020, speed_min, speed_max);

    //handle the orientation switching (optional, you can comment it out if not needed)
    handleOrientation();    
        
    //draw all playfield elements
    drawPattern(bricks, 3);
    drawBall();
    if (Paddle != Paddle_Old)
        drawPaddle(Paddle_Old, false);
    drawPaddle(Paddle, true);
    Paddle_Old = Paddle;
    
    //collision detection with playfield
    if (BallY < 3) //performance opt., unclear if lazy eval works, so two statements
        if (bricks[BallY][BallX] == 1)
        {
            noise(500, 500, 3);

            bricks[BallY][BallX] = 0;    //the hit brick disappears
            BallDY = 1;                  //ball reflects and starts to move down again
            
            //x-reflection of the ball while respecting the playfield boundaries
            if (BallX == 0)
                BallDX = 1;
            else if (BallX == 7)
                BallDX = -1;
            else
                BallDX = random(10) < 5 ? 1 : -1;
                
            //check if the game is won
            boolean won = true; //asume the game is won
            byte wy = 0;
            while (won && wy < 3)
            {
                byte wx = 0;                    
                while (won && wx < 8)
                {
                    if (bricks[wy][wx] == 1)
                        won = false; //a single remaining bricks leads to continued playing
                    wx++;
                }
                wy++;
            }
            
            //end screen and "applause"
            //endless loop: game can be restarted only by a reset
            if (won)
            {
                drawPattern(smiley_won, 8);
                while (true)
                    noise(300, 500, 1000);
            }
        }
        
    //calculate ball movement
    //the ball movement is de-coupled from the paddle movement, i.e. a lower ball speed
    //does NOT lower the paddle movement speed
    if (timer++ >= Speed)
    {
        //RelativePaddleSpeed is important, if you need to give the ball a special
        //"spin" either to left or to right in cases it moves to uniformly and you
        //cannot reach certain bricks
        RelativePaddleSpeed = Paddle - LastPaddlePos;
        LastPaddlePos = Paddle;
        
        //reset the timer
        timer = 0;
        
        //calculate ball movement
        BallX_Old = BallX;
        BallY_Old = BallY;
        BallX += BallDX;
        BallY += BallDY;
        
        //reflection at the LEFT playfield side
        if (BallX == 0)
        {
            BallDX = 1;
            noise(100, 100, 2);
        }

        //reflection at the RIGHT playfield side            
        if (BallX == 7)
        {
            BallDX = -1;
            noise(100, 100, 2);
        }
            
        //reflection at the TOP of the playfield
        if (BallY == 0)
        {
            BallDY = 1;
            noise(100, 100, 2);
        }
                
        //ball hits the paddle
        if (BallY == 6)
        {
            //if the ball is coming from the left (and moving to the right)...
            if (BallDX > 0)
            {
                //... and if the ball hits the paddle
                if (BallX + 1 == Paddle || BallX  == Paddle || BallX - 1 == Paddle)
                {
                    BallDY = -1; // .. then reflect it upwards
                    
                    //and if it hit the paddle at the very left, then also reflect it left
                    if (BallX + 1 == Paddle && BallX != 0)
                        BallDX = -BallDX;
                        
                    //make a short clicking noise when the paddle was hit
                    noise(200, 200, 6);
                }
            }
            
            //the same game as shown above - but now, the ball is coming from the right
            else
            {
                if (BallX - 2 == Paddle || BallX - 1 == Paddle || BallX == Paddle)
                {
                    BallDY = -1;
                    if (BallX - 2 == Paddle && BallX != 7)
                        BallDX = -BallDX;
                    noise(200, 200, 6);
                }
            }
            
            //possibility to influence the ball with a speedy paddle movement
            //you can give it a "spin" either to the left or to the right
            if (abs(RelativePaddleSpeed) >= PaddleSpeedThreshold)
            {
                if (BallX < 6 && RelativePaddleSpeed > 0)
                    BallX++;
                
                if (BallX > 1 && RelativePaddleSpeed < 0)
                    BallX--;
            }
        }
        
        //lost ball
        if (BallY == 8)
        {
            noise(7000, 10000, 100);
            Balls--;
            
            //if there are balls left: respawn
            if (Balls)
            {
                randomBall();
                BallDX = 0;
                BallDY = 0;
                respawn_timer = millis() + respawn_duration;
            }
            
            //otherwise: show the sad smiley and go to an
            //endless loop that can be only left by resetting the system
            else
            {
                displayBallsLeft();
                drawPattern(smiley_lost, 8);                
                while (true) ;
            }
        }
    }
    
    //respawn management
    if (BallDX_tbs != 0)
    {
        if (millis() >= respawn_timer)
        {
            BallDX = BallDX_tbs;
            BallDY = BallDY_tbs;
            BallDX_tbs = BallDY_tbs = respawn_timer = 0;
        }
    }
    
    //change the brightness of the LEDs
    if (Intensity != Intensity_Old)
    {
        Matrix.setIntensity(0, Intensity);
        Intensity_Old = Intensity;
    }
 
    //show remaining balls using LEDs   
    displayBallsLeft();
}