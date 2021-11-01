// ====== include libraries ========
#include <Keypad.h>          // keypad library
#include <FastLED.h>         // Led strip library
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // SPI library
#include <SD.h>              // SD Card library

// ===== key pad settings =====
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {1, 5, 9,  13},
  {2, 6, 10, 14},
  {3, 7, 11, 15},
  {4, 8, 12, 16}
};
byte rowPins[ROWS] = {23, 25, 27, 29};   //connect to the row pinouts of the keypad
byte colPins[COLS] = {31, 33, 35, 37}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//  ====== fastled settings ======
#define LED_PIN     2
#define NUM_LEDS    77
#define BRIGHTNESS  100
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define COLOR_BLACK  0
#define COLOR_RED    1
#define COLOR_BLUE   2
#define COLOR_WHITE  3
#define COLOR_GREEN  4
#define COLOR_YELLOW 5

// ====== LCD and SD card settings ======
#define TFT_RST  44         // Reset line for TFT (or see below...)
#define TFT_CS   46          // Chip select line for TFT display
#define TFT_DC   42          // Data/command line for TFT
#define SD_CS    49         // Chip select line for SD card
//D51 -> MOSI SD + SDA TFT
//D50 -> MISO SD
//D52 -> SCK SD + SCL TFT
#define BUFFPIXEL 20
// LCD object for 1.54" TFT with ST7789 controller
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS,  TFT_DC, TFT_RST);

// ======= red and green button ======
#define BUTTON_RED_PIN     45
#define BUTTON_GREEN_PIN   43

// ======== piezo buzzer =====
#define USE_BUZZER true
#define BUZZER_PIN 41
#define TONE_DEFAULT       0
#define TONE_BUTTON_USER_A 1
#define TONE_BUTTON_USER_B 2

// ======= information table =============
#define BUTTON 0
#define SEN 1
#define SQUARE 2
#define BORDER_RIGHT 3
#define BORDER_LEFT 4
#define BORDER_TOP 5
#define BORDER_BOTTOM 6

/*
  index :
  0 = Type
  1 = Button number
  2 = sen user  どのプレーヤーが線を獲得したか
  3 = led color LEDの色
  4 = sen check1　隣のボタンのIndex (1)
  5 = sen check2  隣のボタンのIndex (2)
  6 = sen check3  隣のボタンのIndex (3)
  7 = sen check4  隣のボタンのIndex (4)
  8 = sen1  BUTTONのTypeの場合：sen check1 の隣のボタンとの間位の線のIndex
             SQUAREのTypeの場合：周りのBlockのIndex（1）
  9 = sen2　BUTTONのTypeの場合：sen check2 の隣のボタンとの間位の線のIndex
             SQUAREのTypeの場合：周りのBlockのIndex（2）
  10 = sen3　BUTTONのTypeの場合：sen check3 の隣のボタンとの間位の線のIndex
             SQUAREのTypeの場合：周りのBlockのIndex（3）
  11 = sen4　BUTTONのTypeの場合：sen check4 の隣のボタンとの間位の線のIndex
             SQUAREのTypeの場合：周りのBlockのIndex（4）
  12 = square1  このIndexと関係しているSquare (1)
  13 = square2　このIndexと関係しているSquare (2)
  14 = square3　このIndexと関係しているSquare (3)
  15 = square4　このIndexと関係しているSquare (4)
*/

int infotable[77][16] = {
  {BUTTON,        13, 255, COLOR_BLACK,   2, 14, 255, 255,   1, 13, 255, 255,  12, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  12, 255, 255, 255},
  {BUTTON,         9, 255, COLOR_BLACK,   0,  4, 16, 255,   1,  3, 11, 255,  10, 12, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  10, 255, 255, 255},
  {BUTTON,         5, 255, COLOR_BLACK,   2,  6, 18, 255,   3,  5,  9, 255,   8, 10, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,   8, 255, 255, 255},
  {BUTTON,         1, 255, COLOR_BLACK,   4, 20, 255, 255,   5,  7, 255, 255,   8, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,   8, 255, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,   5,  7,  9, 19, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,   8, 10, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,   3,  9, 11, 17, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  10, 12, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,   1, 11, 13, 15, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  12, 255, 255, 255},
  {BUTTON,        14, 255, COLOR_BLACK,   0, 16, 28, 255,  13, 15, 27, 255,  12, 26, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  12, 26, 255, 255},
  {BUTTON,        10, 255, COLOR_BLACK,   2, 14, 18, 30,  11, 15, 17, 25,  10, 12, 24, 26},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  10, 24, 255, 255},
  {BUTTON,         6, 255, COLOR_BLACK,   4, 16, 20, 32,   9, 17, 19, 23,   8, 10, 22, 24},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,   8, 22, 255, 255},
  {BUTTON,         2, 255, COLOR_BLACK,   6, 18, 34, 255,   7, 19, 21, 255,   8, 22, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  22, 255, 255, 255},
  {SQUARE,      255, 255, COLOR_BLACK, 255, 255, 255, 255,  19, 21, 23, 33, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  22, 24, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,  17, 23, 25, 31, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  24, 26, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,  15, 25, 27, 29, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  26, 255, 255, 255},
  {BUTTON,        15, 255, COLOR_BLACK,  14, 30, 42, 255,  27, 29, 41, 255,  26, 40, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  26, 40, 255, 255},
  {BUTTON,        11, 255, COLOR_BLACK,  16, 28, 32, 44,  25, 29, 31, 39,  24, 26, 38, 40},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  24, 38, 255, 255},
  {BUTTON,         7, 255, COLOR_BLACK,  18, 30, 34, 46,  23, 31, 33, 37,  22, 24, 36, 38},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  22, 36, 255, 255},
  {BUTTON,         3, 255, COLOR_BLACK,  20, 32, 48, 255,  21, 33, 35, 255,  22, 36, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  36, 255, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,  33, 35, 37, 47, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  36, 38, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,  31, 37, 39, 45, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  38, 40, 255, 255},
  {SQUARE,       255, 255, COLOR_BLACK, 255, 255, 255, 255,  29, 39, 41, 43, 255, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  40, 255, 255, 255},
  {BUTTON,        16, 255, COLOR_BLACK,  28, 44, 255, 255,  41, 43, 255, 255,  40, 255, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  40, 255, 255, 255},
  {BUTTON,        12, 255, COLOR_BLACK,  30, 42, 46, 255,  39, 43, 45, 255,  38, 40, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  38, 255, 255, 255},
  {BUTTON,         8, 255, COLOR_BLACK,  32, 44, 48, 255,  37, 45, 47, 255,  36, 38, 255, 255},
  {SEN,        255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255,  36, 255, 255, 255},
  {BUTTON,         4, 255, COLOR_BLACK,  34, 46, 255, 255,  35, 47, 255, 255,  36, 255, 255, 255},
  {BORDER_RIGHT, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_RIGHT, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_RIGHT, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_RIGHT, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_RIGHT, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_RIGHT, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_RIGHT, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_BOTTOM, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_BOTTOM, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_BOTTOM, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_BOTTOM, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_BOTTOM, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_BOTTOM, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_BOTTOM, 255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_LEFT,  255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_LEFT,  255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_LEFT,  255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_LEFT,  255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_LEFT,  255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_LEFT,  255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_LEFT,  255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_TOP,   255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_TOP,   255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_TOP,   255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_TOP,   255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_TOP,   255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_TOP,   255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
  {BORDER_TOP,   255, 255, COLOR_BLACK, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}
};
// ======= application settings ===========
#define GAME_STATE_WAITTOSTART 1
#define GAME_STATE_PLAYING     2
#define GAME_STATE_FINISHED    3
#define GAME_STATE_RESET       4

#define PLAYER1 1
#define PLAYER2 2

int gamestate = GAME_STATE_WAITTOSTART;
int ledindex = 0;
int currentplayer = PLAYER1;
int firstbutton = 0;
int secondbutton = 0;

bool debug_messages = true;
int lastLedTotal = -1;

// ======= application methods ===========
void setup()
{
  // set up serial monitor
  Serial.begin(9600);
  delay(3000) ;
  while (!Serial)
  {
    delay(10);  // wait for serial console
  }
  Serial.println("Initialization started");

  // initialize LCD
  Serial.println("Initializing LCD");
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
  tft.fillScreen(ST77XX_BLACK);

  // initialize SD card
  Serial.print("Initializing SD card");
  if (!SD.begin(SD_CS))
  {
    Serial.println("Initialization of SD card failed!");
  }

  // initialize buzzer
  Serial.println("Initializing Buzzer");
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  // initialize red and green buttons
  pinMode(BUTTON_RED_PIN,   INPUT_PULLUP); // pull high if button not pressed
  pinMode(BUTTON_GREEN_PIN, INPUT_PULLUP); // pull high if button not pressed

  // initialize led strip
  Serial.println("Initializing led strip");
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  FastLED.setMaxPowerInMilliWatts(500);

  // initialize application
  Serial.println("Initialize application");
  initializeApplication();

  // initialization finished
  Serial.println("Initialization finished");

}

void loop()
{
  runGameLoop();

  // update LEDS based on infotable field 3
  updateLEDS();
}

void initializeApplication()
{
  // show logo
  //bmpDraw("dotblock.bmp", 0, 0);
  screen_DrawOpeningImage();
}

void changePlayer()
{
  if (debug_messages) { Serial.println("Change player"); }
  if (currentplayer == PLAYER1)
  {
    if (debug_messages) { Serial.println("Set to PLAYER 2"); }
    currentplayer = PLAYER2;
  }
  else
  {
    if (debug_messages) { Serial.println("Set to PLAYER 1");
    }
    currentplayer = PLAYER1;
  }

  // clear button pressed information
  firstbutton  = 0;
  secondbutton = 0;

  // clear button lights
  clearButtonLights();

  // light up border for current player
  lightUpBorderCurrentPlayer();

  // light up all buttons for current player
  lightUpAllButtonsCurrentPlayer();

  // update score screen
  doShowScore();
}

void runGameLoop()
{
  if (gamestate == GAME_STATE_WAITTOSTART)
  {
    // check to start game
    doCheckStartButton();
  }
  else if (gamestate == GAME_STATE_PLAYING)
  {
    // play game
    doPlayGame();
  }

  // check if reset button is pressed
  doCheckResetButton();
}

void doCheckResetButton()
{
  if(checkButtonRedPressed())
  {
    resetGame();
  }
}

void doCheckStartButton()
{
  if(checkButtonGreenPressed())
  {
    Serial.println("Start button pressed : starting game");
    // start game
    gamestate = GAME_STATE_PLAYING;

    // set current user
    currentplayer = PLAYER1;

    // light up border for current player
    lightUpBorderCurrentPlayer();

    // light up all buttons for current player
    lightUpAllButtonsCurrentPlayer();

    // show score
    doShowScore();
  }
}

void doShowScore()
{
    tft.setRotation(1);
  // set background color to current player
  if (currentplayer == PLAYER1)
  {
    // make background blue
     tft.fillScreen(ST77XX_BLUE);
  }
  else
  {
    // make background red
     tft.fillScreen(ST77XX_RED);
  }

  // print SCORE
  tft.setCursor(40, 40);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(5);
  tft.print("SCORE");

  // draw black boxes
  tft.fillRect(25, 85, 70, 70, ST77XX_BLACK);
  tft.fillRect(25+120, 85, 70, 70, ST77XX_BLACK);

  // draw player 1 score
  int i;
  int player1_squares = 0;
  int player2_squares = 0;
  for (i = 0; i < 77; i++)
  {
    if (infotable[i][0] == SQUARE)
    {
      if (infotable[i][2] == PLAYER1)
      {
        player1_squares++;
      }
      else if (infotable[i][2] == PLAYER2)
      {
        player2_squares++;
      }
    }
  }
  tft.setCursor(25+11, 85+3);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(9);
  tft.print(player1_squares);

  // draw player 2 score
  tft.setCursor(145+11, 85+3);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(9);
  tft.print(player2_squares);
}

void doShowWinningPlayer(int winningplayer)
{
    tft.setRotation(1);
  // set background color to winning player
  if (winningplayer == PLAYER1)
  {
    // make background blue
     tft.fillScreen(ST77XX_BLUE);
  }
  else
  {
    // make background red
     tft.fillScreen(ST77XX_RED);
  }

  // print SCORE
  tft.setCursor(40, 40);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(5);
  tft.print("SCORE");

  // draw black boxes
  tft.fillRect(25, 85, 70, 70, ST77XX_BLACK);
  tft.fillRect(25+120, 85, 70, 70, ST77XX_BLACK);

  // draw player 1 score
  int i;
  int player1_squares = 0;
  int player2_squares = 0;
  for (i = 0; i < 77; i++)
  {
    if (infotable[i][0] == SQUARE)
    {
      if (infotable[i][2] == PLAYER1)
      {
        player1_squares++;
      }
      else if (infotable[i][2] == PLAYER2)
      {
        player2_squares++;
      }
    }
  }
  tft.setCursor(25+11, 85+3);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(9);
  tft.print(player1_squares);

  // draw player 2 score
  tft.setCursor(145+11, 85+3);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(9);
  tft.print(player2_squares);

  // show who won
  if (winningplayer == PLAYER1)
  {
     tft.setCursor(15, 160);
     tft.setTextColor(ST77XX_WHITE);
     tft.setTextSize(4);
     tft.print("WIN  LOSE");
  }
  else
  {
    tft.setCursor(15, 160);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(4);
    tft.print("LOSE WIN");
  }
}

void resetGame()
{
  // set all led to black
  int i;
  for (i = 0; i < 77; i++)
  {
    infotable[i][3] = COLOR_BLACK;
    infotable[i][2] =
  }

  // reset game state
  gamestate = GAME_STATE_WAITTOSTART;

  // show first screen
  screen_DrawOpeningImage();
}

void doPlayGame()
{
  // key pressed?
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    // first time button pressed?
    if (firstbutton == 0)
    {
      if (debug_messages) { Serial.print("First button pressed:"); printKey(key); }
      // no buttons pressed
      firstbutton = key;
      lightUpButtonLED(key);
    }
    else
    {
      // button1 was already pressed
      // this is the second button
      secondbutton = key;

      if (debug_messages) { Serial.print("Second button pressed:");printKey(key); }
      // 1. check if first and second button are next to eachother
      if (checkValidChoice())
      {
        if (debug_messages) {
          Serial.println("Valid choice");
        }
        // 2. まだ線が黒であるか？
        if (isSenBewteenFirstAndSecondButtonBlack())
        {
          if (debug_messages) { Serial.println("Sen between buttons is black"); }
          // 3. 線が黒だった。

          // 4. 線は現在プレーヤーの色をつける
          if (debug_messages) { Serial.println("Give Sen to Player"); }
          giveSenToPlayer();

          // 5. Squareが作れるかどうかのチェック
          if (debug_messages) { Serial.println("Check if we can make a square"); }
          bool userMadeSquare;
          userMadeSquare = MakeSquare();
          if (userMadeSquare)
          {
            // user made square : show new score
            doShowScore();
          }
          // 6. すべてのSquareができたか
          if (debug_messages) { Serial.println("Check if game has finished"); }
          if (checkGameFinished())
          {
            if (debug_messages) { Serial.println("Game finished"); }
            // 7. ゲーム終了
            showWinningPlayer();
            // 8. game_stateをFinishedに変える。
            gamestate = GAME_STATE_FINISHED;
          }
          else
          {
            if (debug_messages) { Serial.println("Game not finished"); }
            // 8. もう一回現在のプレーヤーのばんだ。
            // first button clear
            // second button clear
            firstbutton  = 0;
            secondbutton = 0;
            // clear button lights
            clearButtonLights();
            if (userMadeSquare)
            {
              // Squareができたから、同じプレーヤーがもう一度線を引く
            }
            else
            {
              // Squareができなかったから、交代する。
              changePlayer();

            }
          }
        }
        else
        {
          // invalid choice -> 線はすでにほかのプレーヤーに獲得した。
          if (debug_messages) { Serial.println("Sen between buttons is owned by other player"); }
          changePlayer();
        }
      }
      else
      {
        // invalid choice -> change current player
        if (debug_messages) { Serial.println("Invalid choice"); }
        changePlayer();
      }
    }
  }
}

// ================ 画面の処理の関数 =============
void screen_DrawOpeningImage()
{
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(5);
  tft.setTextWrap(true);

  tft.setCursor(75, 25);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("DOT");

    tft.setCursor(110,75);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("&");

  tft.setCursor(75, 125);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("BOX");

  tft.setTextSize(3);
  tft.setCursor(15, 175);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("Press Start");
}

// ================ 画面の処理の関数 =============

bool isSenBewteenFirstAndSecondButtonBlack()
{
// first button
  int ledFirstButton = getLedForButton(firstbutton);

  // second button
  int ledSecondButton = getLedForButton(secondbutton);

  // 最初のボタンと次のボタンの間の線を特定する
  int senLed;
  int i;
  for (i = 4; i <= 7; i++)
  {
    if (infotable[ledFirstButton][i] == ledSecondButton)
    {https://create.arduino.cc/
      senLed = infotable[ledFirstButton][i + 4];
      break;
    }
  }

  if ((infotable[senLed][2] != PLAYER1) && (infotable[senLed][2] != PLAYER2))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void giveSenToPlayer()
{
  // first button
  int ledFirstButton = getLedForButton(firstbutton);

  // second button
  int ledSecondButton = getLedForButton(secondbutton);

  // 最初のボタンと次のボタンの間の線を特定する
  int senLed;
  int i;
  for (i = 4; i <= 7; i++)
  {
    if (infotable[ledFirstButton][i] == ledSecondButton)
    {
      senLed = infotable[ledFirstButton][i + 4];
      break;
    }
  }
  // 線を現在のプレーヤーの色にする　（3）
  int color;
  if (currentplayer == PLAYER1)
  {
    color = COLOR_BLUE;
  }
  else
  {
    color = COLOR_RED;

  }
  infotable[senLed][3] = color;

  // 線を現在のプレーヤーのものにする　（2）
  infotable[senLed][2] = currentplayer;
}

bool MakeSquare()
{
  bool userMadeSquare = false;

  // SQUAREを探す
  int i;
  for (i = 0; i < 77; i++)
  {
    if (infotable[i][0] == SQUARE)
    {
      if (infotable[i][2] == 255)
      {
        // 周りの線が全部光っているか？
        int tonariNoSen = infotable[i][8];
        if ((infotable[tonariNoSen][2] == PLAYER1) ||
            (infotable[tonariNoSen][2] == PLAYER2))
        {
          tonariNoSen = infotable[i][9];
          if ((infotable[tonariNoSen][2] == PLAYER1) ||
              (infotable[tonariNoSen][2] == PLAYER2))
          {
            tonariNoSen = infotable[i][10];
            if ((infotable[tonariNoSen][2] == PLAYER1) ||
                (infotable[tonariNoSen][2] == PLAYER2))
            {
              tonariNoSen = infotable[i][11];
              if ((infotable[tonariNoSen][2] == PLAYER1) ||
                  (infotable[tonariNoSen][2] == PLAYER2))
              {
                userMadeSquare = true;
                // すべての線が光っているー＞SQUAREができる
                int color;
                if (currentplayer == PLAYER1)
                {
                  color = COLOR_BLUE;
                  if (debug_messages) { Serial.print("Give Square ");Serial.print(i);Serial.println("to PLAYER 1 "); }
                }
                else
                {
                  color = COLOR_RED;
                  if (debug_messages) { Serial.print("Give Square ");Serial.print(i);Serial.println("to PLAYER 2 "); }
                }
                infotable[i][3] = color;

                // SQUAREを現在のプレーヤーのものにする　（2）
                infotable[i][2] = currentplayer;
              }
            }
          }
        }
      }
    }
  }
  return userMadeSquare;
}

boolean checkGameFinished()
{
  // SQUAREを探す
  int i;
  int squaresLightedUp = 0;

  for (i = 0; i < 77; i++)
  {
    if (infotable[i][0] == SQUARE)
    {
      if ((infotable[i][2] == PLAYER1) ||
          (infotable[i][2] == PLAYER2))
      {
        squaresLightedUp++;
      }
    }
  }
  if (squaresLightedUp == 9)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void showWinningPlayer()
{
  if (debug_messages) { Serial.println("Show winning player"); }
  int i;
  int player1_squares = 0;
  int player2_squares = 0;
  for (i = 0; i < 77; i++)
  {
    if (infotable[i][0] == SQUARE)
    {
      if (infotable[i][2] == PLAYER1)
      {
        player1_squares++;
      }
      else if (infotable[i][2] == PLAYER2)
      {
        player2_squares++;
      }
    }
  }
  int winnerColor;
  if (player1_squares > player2_squares)
  {
    if (debug_messages) { Serial.println("PLAYER 1 (blue) won!"); }
    winnerColor = COLOR_BLUE;
    doShowWinningPlayer(PLAYER1);
  }
  else
  {
    if (debug_messages) { Serial.println("PLAYER 2 (red) won!"); }
    winnerColor = COLOR_RED;
    doShowWinningPlayer(PLAYER2);
  }

  // make all non-SQUARE leds the winning color
  for (i = 0; i < 77; i++)
  {
    if (infotable[i][0] != SQUARE)
    {
        infotable[i][3] = winnerColor;
    }
  }
 }

bool checkValidChoice()
{
  // firstbutton
  int ledFirstButton = getLedForButton(firstbutton);

  // second button
  int ledSecondButton = getLedForButton(secondbutton);

  if (checkIsFirstButtonNextToSecondButton(ledFirstButton, ledSecondButton))
  {
    // valid choice : OK
    return true;
  }
  else
  {
    // invalid choice : WRONG
    return false;
  }

}

bool checkIsFirstButtonNextToSecondButton(int ledFirstButtonRowIndex, int ledSecondButtonRowIndex)
{
  // 最初のボタンLEDのblockcheck1がledSecondButtonであることをチェックする
  if (infotable[ledFirstButtonRowIndex][4] == ledSecondButtonRowIndex)
  {
    return true;
  }
  if (infotable[ledFirstButtonRowIndex][5] == ledSecondButtonRowIndex)
  {
    return true;
  }
  if (infotable[ledFirstButtonRowIndex][6] == ledSecondButtonRowIndex)
  {
    return true;
  }
  if (infotable[ledFirstButtonRowIndex][7] == ledSecondButtonRowIndex)
  {
    return true;
  }
  return false;
}

int getLedForButton(char key)
{
  int i;
  for (i = 0; i <= 76; i++)
  {
    int button = infotable[i][1];
    if (button == key)
    {
      return i;
    }
  }
  return 255;
}


void lightUpAllButtonsCurrentPlayer()
{
  // TO DO
  if (debug_messages) { Serial.println("light up all buttons for current player"); }
  int i,j, player,maxcoloredcount,coloredcount, sen_index;
  for (i = 0; i <= 76; i++)
  {
    if (infotable[i][0] == BUTTON)
    {
       maxcoloredcount = 4;
       coloredcount = 0;
       for(j=0;j<=3;j++)
       {
          sen_index = infotable[i][8+j];
          if (sen_index != 255)
          {
             player = infotable[sen_index][2];
             if (player == PLAYER1 || player == PLAYER2)
             {
               coloredcount++;
             }
          }
          else
          {
             maxcoloredcount--;
          }
       }

       if (coloredcount != maxcoloredcount)
       {
           // button can be pressed
           infotable[i][3] = COLOR_WHITE;
       }
       else
       {
          infotable[i][3] = COLOR_BLACK;
       }
    }
  }

}

void updateLEDS()
{
  int i;
  int color;
  int total = 0;
  for (i = 0; i <= 76; i++)
  {
    color = infotable[i][3];
    if (color == COLOR_BLACK)
    {
      leds[i] = CRGB::Black;
    }
    else if (color == COLOR_RED)
    {
      leds[i] = CRGB::Red;
    }
    else if (color == COLOR_BLUE)
    {
      leds[i] = CRGB::Blue;
    }
    else if (color == COLOR_GREEN)
    {
      leds[i] = CRGB::Green;
    }
    else if (color == COLOR_YELLOW)
    {
      leds[i] = CRGB::Yellow;
    }
    else
    {
      leds[i] = CRGB::White;
    }
    total += color;
  }

  if (total != lastLedTotal)
  {
    //if (debug_messages) { Serial.print("Update leds");}
    lastLedTotal = total;
    FastLED.show();
  }
}

void clearButtonLights()
{
  int i;
  for (i = 0; i <= 76; i++)
  {
    if (infotable[i][0] == BUTTON)
    {
      infotable[i][0] = COLOR_BLACK;
    }
  }
}

void lightUpButtonLED(char key)
{
  int i;
  for (i = 0; i <= 76; i++)
  {
    int button = infotable[i][1];
    if (button == key)
    {
      int color;
      if (currentplayer == PLAYER1)
      {
        color = COLOR_BLUE;
      }
      else
      {
        color = COLOR_RED;
Serial.println("Color set to RED");
      }
      infotable[i][3] = color;
    }
  }
}

void lightUpBorderCurrentPlayer()
{
  if (currentplayer == PLAYER1)
  {
    if (debug_messages) { Serial.println("Make border blue for player 1."); }
    lightUpBorder(COLOR_BLUE);
  }
  else
  {
    if (debug_messages) { Serial.println("Make border red for player 2."); }
    lightUpBorder(COLOR_RED);
  }
}

void printKey(int key)
{
  if (key < 10)
  {
    Serial.println((char)(48 + key));
  }
  else
  {
    Serial.println((char)(55 + key));
  }
}

/*
void setLEDColor(int ledindex, CRGB color)
{
  if (ledindex < NUM_LEDS)
  {
    leds[ledindex] = color;
  }
}
*/

void lightUpBorder(int color)
{

  if (debug_messages) { Serial.print("Set border color to ");Serial.println(color); }
  for (int i = 49; i <= 76; i++)
  {
    infotable[i][3] = color;
  }
}

// ===========================================================
// TFT and SD methods
// ===========================================================
// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.
void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42)
  {
    // BMP signature
    //Serial.print(F("File size: "));
    //Serial.println(
    (void)read32(bmpFile);//);
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    //Serial.print(F("Image Offset: "));
    //Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    //Serial.print(F("Header size: "));
    //Serial.println(
    read32(bmpFile);//);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1)
    { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      //Serial.print(F("Bit Depth: "));
      //Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0))
      { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        //Serial.print(F("Image size: "));
        //Serial.print(bmpWidth);
        //Serial.print('x');
        //Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0)
        {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width())  w = tft.width()  - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;
tft.startWrite();
        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (row = 0; row < h; row++)
        { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip)
          {
            // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          }
          else
          {
            // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          }

          if (bmpFile.position() != pos)
          { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++)
          { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer))
            { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            //tft.pushColor(tft.color565(r, g, b));
tft.writePixel(h-row,col,tft.color565(r, g, b));
          } // end pixel
        } // end scanline
tft.endWrite();
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println(F("BMP format not recognized."));
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


void printDirectory(File dir, int numTabs)
{
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void playTone(int toneType)
{
  if (USE_BUZZER)
  {
    unsigned long duration;
    unsigned int frequency;
    if (toneType == TONE_BUTTON_USER_A)
    {
      frequency = 500;
      duration = 200;
    }
    else if (toneType == TONE_BUTTON_USER_B)
    {
      frequency = 1000;
      duration = 200;
    }
    else if (toneType == TONE_DEFAULT)
    {
      frequency = 2500;
      duration = 500;
    }
    else
    {
      frequency = 2500;
      duration = 500;
    }
    tone(BUZZER_PIN, frequency, duration);
  }
  else
  {
    Serial.print("Sound muted");
  }
}

void checkRedGreenButton()
{
  if (checkButtonRedPressed())
  {
    Serial.println("Red button pressed");
  }

  if (checkButtonGreenPressed())
  {
    Serial.println("Green button pressed");
  }
}

boolean checkButtonRedPressed()
{
  if (digitalRead(BUTTON_RED_PIN) == LOW)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean checkButtonGreenPressed()
{
  return (digitalRead(BUTTON_GREEN_PIN) == LOW);
}
