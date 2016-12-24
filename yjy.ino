#include <Adafruit_NeoPixel.h>
#include <Microduino_Key.h>
#include <Microduino_Motor.h>
 #include "U8glib.h"
#if defined(__AVR_ATmega32U4__) || (__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define motor_pin0A 8  //PWM
#define motor_pin0B 6
#define motor_pin1A 7  //PWM 
#define motor_pin1B 5
#else
#define motor_pin0A 6  //PWM
#define motor_pin0B 8
#define motor_pin1A 5  //PWM
#define motor_pin1B 7
#endif
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
 
Key KeyA(A6, INPUT);
Key KeyB(A6, INPUT);
Key KeyC(A6, INPUT);
Key KeyD(A6, INPUT);
Key KeyE(A6, INPUT);
 
Motor MotorLeft(motor_pin0A, motor_pin0B);
Motor MotorRight(motor_pin1A, motor_pin1B);
 
#define MAX_THROTTLE 255 //最大油门 100~255
#define MAX_STEERING 200 //最大转向 100~512
 
int16_t leftVal = 0;  //左边电机旋转速度
int16_t rightVal = 0;  //右边电机旋转速度
 
#include "key.h"

#define PIXEL_PIN    A0    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT  6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define setFont_L u8g.setFont(u8g_font_9x18)

int key_Pin = 4;
int speakerPin = 6;

boolean play_pause;

long timer1, timer2;
int val, add;

#define val_max 255
#define val_min 0

// notes to play; see Tone.h for frequencies;
int notes[] = {
  NOTE_G5, NOTE_C6, NOTE_C6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_A5, NOTE_A5,
  NOTE_A5, NOTE_D6, NOTE_D6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_B5, NOTE_G5,
  NOTE_G5, NOTE_E6, NOTE_E6, NOTE_F6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_A5,
  NOTE_G5, NOTE_G5, NOTE_A5, NOTE_D6, NOTE_B5, NOTE_C6,
  NOTE_G5, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_B5, 
  NOTE_B5, NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G5,
  NOTE_D6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_G7, NOTE_G5,
  NOTE_G5, NOTE_G5, NOTE_A5, NOTE_D6, NOTE_B5, NOTE_C6
};

// number of beats for each note
int beats[] = {
  2, 2, 1, 1, 1, 1, 2, 2,
  2, 2, 1, 1, 1, 1, 2, 2,
  2, 2, 1, 1, 1, 1, 2, 2,
  1, 1, 2, 2, 2, 4,
  2, 2, 2, 2, 4,
  2, 2, 2, 2, 4,
  2, 2, 2, 2, 2, 2,
  1, 1, 2, 2, 2, 4
};



// Calculate song length
int songLength = sizeof(notes) / sizeof(int);
int tempo = 220; // in milliseconds



void playNote(int note, int beat) {
  noTone(speakerPin); // speaker reset
  tone(speakerPin, note); // play tone
  if (millis() != timer1) timer1 = millis();
  while (millis() - timer1 <= tempo * beat)
    blink();
  noTone(speakerPin); // speaker reset
  if (millis() != timer2) timer2 = millis();
  while (millis() - timer2 <= tempo / 4)
    blink();
}

void setup() {
  Serial.begin(115200);
  pinMode(key_Pin, INPUT);
  key_init();
  strip.begin();
  strip.show();
  MotorLeft.Fix(1);
  MotorRight.Fix(1);
  
}

void loop() {
  blink();
   u8g.firstPage();
    do {
      draw();
    }
    while ( u8g.nextPage() );
  if (play_pause)
  {
    for (int i = 0; i < songLength; i++) {
      add++;
      if (add == 5)
        add = 1;
      if (add == 1)
        colorSet(strip.Color(i * 10, 0, 0));
      else if (add == 2)
        colorSet(strip.Color(0, i * 10, 0));
      else if (add == 3)
        colorSet(strip.Color(0, 0, i * 10));
      else if (add == 4)
        colorSet(strip.Color(i * 10, i * 10, 0));
      if (!play_pause)
      {
        play_pause = false;
        noTone(speakerPin);
        return;
      }
      playNote(notes[i], beats[i]); // make sound
    }
  }
  else
  {
    noTone(speakerPin);
    colorSet(0);
  }
}

void blink()
{
  if (key_get(key_Pin, 0))
  {
    delay(200);
    play_pause = !play_pause;
  }
}

void colorSet(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
   switch (KeyA.read(680, 720)) {
    case SHORT_PRESS:
      Serial.println("KEY UP(analog) SHORT_PRESS");  //短按
        leftVal+=10;     //摇杆向上，旋转速度加10，最大不超过255
        if (leftVal > 255)
          leftVal = 255;
        rightVal+=10;
        if (rightVal > 255)
          rightVal = 255;
      break;
    case LONG_PRESS:
      Serial.println("KEY UP(analog) LONG_PRESS");    //长按
      break;
  }
 
  switch (KeyB.read(480, 530)) {
    case SHORT_PRESS:
      Serial.println("KEY LEFT(analog) SHORT_PRESS");  //短按
        if (leftVal > 0)     //摇杆向左，旋转方向向左
          {
            leftVal = -leftVal;
            rightVal = -rightVal;
          }
      break;
    case LONG_PRESS:
      Serial.println("KEY LEFT(analog) LONG_PRESS");    //长按
      break;
  }
 
  switch (KeyC.read(830, 870)) {
    case SHORT_PRESS:
      Serial.println("KEY RIGHT(analog) SHORT_PRESS");  //短按
        if (leftVal < 0)      //摇杆向右，旋转方向向右
          {
            leftVal = -leftVal;
            rightVal = -rightVal;
          }
      break;
    case LONG_PRESS:
      Serial.println("KEY RIGHT(analog) LONG_PRESS");    //长按
      break;
  }
 
  switch (KeyD.read(310, 350)) {
    case SHORT_PRESS:
      Serial.println("KEY DOWN(analog) SHORT_PRESS");  //短按 
        leftVal-=10;    //摇杆向下，旋转速度减10，最小不超过-255
        if (leftVal < -255)
          leftVal = -255;
        rightVal-=10;
        if (rightVal < -255)
          rightVal = -255;
      break;
    case LONG_PRESS:
      Serial.println("KEY DOWN(analog) LONG_PRESS");    //长按
      break;
  }
 
  switch (KeyE.read(0, 20)) {
    case SHORT_PRESS:
      Serial.println("KEY ENTER(analog) SHORT_PRESS");  //短按
      break;    //摇杆向下，没有定义动作
    case LONG_PRESS:
      Serial.println("KEY ENTER(analog) LONG_PRESS");    //长按
      break;
  }
 
  MotorLeft.Driver(leftVal);  //驱动电机
  MotorRight.Driver(rightVal);
  delay(15);
}
void draw()
{
   u8g.setFont(u8g_font_gdr14r);
    u8g.setPrintPos(32, 30);
    u8g.print("Merry");
       u8g.setFont(u8g_font_gdr14r);
        u8g.setPrintPos(10, 60);
    u8g.print("Christmas!");
 
}
