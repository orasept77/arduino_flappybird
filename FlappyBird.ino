#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

// OLED spi 
#define SCLK_PIN 2
#define MOSI_PIN 3
#define OLED_CS    6
#define OLED_RESET 7
#define OLED_DC    5


#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 

#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS, OLED_DC, OLED_RESET); 

class Flappy {
  public:
    Flappy();

    void draw();
    void update();
    void setButtonPin(int pin);
    int getButtonPin();
    void setGravity(int gravity);
    void setJumpSpeed(float jumpSpeed);
    bool isOutOfBounds();

  private:
    int buttonPin;
    const uint8_t shape[8] PROGMEM = {
      B00011000,
      B00111100,
      B01111110,
      B11111111,
      B11111111,
      B01111110,
      B00111100,
      B00011000
    };
    int gravity;
    float jumpSpeed;

  public:
    int x;
    int y;
    int previousY;
    const int size = 8;

};

Flappy::Flappy() {
  buttonPin = 2;
  gravity = 2;
  jumpSpeed = 5;
  x = 20;
  y = SCREEN_HEIGHT / 2 - 8 / 2;
  previousY = y;
}

void Flappy::setButtonPin(int pin) {
  buttonPin = pin;
}

int Flappy::getButtonPin() {
  return buttonPin;
}

void Flappy::setGravity(int gravity) {
  gravity = gravity;
}

void Flappy::setJumpSpeed(float _jumpSpeed) {
  jumpSpeed = _jumpSpeed;
}

void Flappy::draw() {
  // delete previous track for performance optimization
  display.fillRect(x, previousY, size, size, BLACK);

  // draw bird
  display.drawBitmap(x, y, shape, size, size, YELLOW);

  // previous position of the bird
  previousY = y;
}

void Flappy::update() {
  // move bird
  if (digitalRead(buttonPin) == LOW) { //if low -> fly bird
    previousY = y;
    y -= jumpSpeed;
  } else {
    y += gravity;
  }
}

bool Flappy::isOutOfBounds() {
  return y < 0 || y + size > 128;
}

class Obstacle {
  public:
    Obstacle();

    void draw();
    void update();
    void setSpeed(float speed);
    bool isOutOfBounds();
    void setXY(int x, int y);
    int getX();
    int getY();

  private:
    int x;
    int y;

  public:
    const int width = 10;
    const int gap = 40;
    float speed;
};

Obstacle::Obstacle() {
  speed = 3;
  x = 128;
  y = 64;
}

void Obstacle::setSpeed(float speed) {
  this->speed = speed;
}

void Obstacle::setXY(int x, int y)
{
  this->x = x;
  this->y = y;
}



void Obstacle::draw() {
  // delete area of the screen occupied by the obstacle
  display.fillRect(x - speed, 0, width + speed * 2, 128, BLACK);
  
  // draw obstacles
  display.fillRect(x, 0, width, y, WHITE);
  display.fillRect(x, y + gap, width, 128 - y - gap, WHITE);
}

void Obstacle::update() {
  x -= speed;
}

bool Obstacle::isOutOfBounds() {
  return x < -width;
}

int Obstacle::getX()
{
  return x;
}

int Obstacle::getY()
{
  return y;
}


class Game{

  public:
    Game();
    void gameOver();

};

    Game::Game() {

}

void Game::gameOver() {
  display.fillScreen(BLACK);
  display.setCursor(35, SCREEN_HEIGHT/2);
  display.setTextColor(WHITE);
  display.print("Game Over!");
  while (true) {}
}

Flappy Flappy;
Obstacle obstacle;
Game game;

void setup() {
  Serial.begin(9600);
  display.begin();
  display.fillScreen(BLACK);
  randomSeed(analogRead(A0));
  pinMode(Flappy.getButtonPin(), INPUT_PULLUP);
  
}


void loop() {
  // delete bird track
  display.fillRect(Flappy.x, Flappy.previousY, Flappy.size, Flappy.size, BLACK);

  // update bird
  Flappy.update();

  // check collision with upper or lower edge of the screen
  if (Flappy.isOutOfBounds()) {
    game.gameOver();
  }

  // move obstacles
  obstacle.update();

  // check if the bird has touched the obstacle
  if (obstacle.getX() < Flappy.x + Flappy.size && obstacle.getX() + obstacle.width > Flappy.x && (Flappy.y < obstacle.getY() || Flappy.y + Flappy.size > obstacle.getY() + obstacle.gap)) {
    game.gameOver();
  }

  // check if the obstacle has left the screen
  if (obstacle.isOutOfBounds()) {
    // create new obstacles
    obstacle.setXY(128, random(20, 108 - obstacle.gap));
    
  }

  // draw bid
  Flappy.draw();

  // draw obstacles
  obstacle.draw();

  // small delay
  delay(20);
}
