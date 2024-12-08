/*
<-Dev Community Project->
Project: Flappy Bird Display Game 
Created: 8/12/2024
Version: 1.0.0
For Arduino Uno, Arduino Nano
<----->
How to connect:
 |ILI9341|->|Arduino|
 | VCC   |->|  5V   | -WANRING : YOU MUST CONNECT PIN J1 UNDER DISPLAY-
 | GND   |->|  GND  |
 | CS    |->|  D10  |
 | RST   |->|  D8   |
 | DC    |->|  D9   |
 | MOSI  |->|  D11  |
 | SCK   |->|  D13  |
 | LED   |->|  3.3V |
 | MISO  |->|  D12  | -Can do not connect to arduino-
---------------------------------------------------------------------\
 |Button1|->|  D2   | For control bird
 |Button2|->|  D4   | For control audio
 | Buzzer|->|  D3   | For Buzzer


 */



#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Các chân kết nối ILI9341
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

// Khai báo màn hình TFT
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Kích thước màn hình TFT
#define TFTW 240
#define TFTH 320

// Thông số Flappy Bird
#define BIRDW 15
#define BIRDH 15
#define PIPEW 30
#define GAPHEIGHT 80
#define GRAVITY 1
#define FLAP_STRENGTH -10

// Màu sắc
#define BCKGRDCOL ILI9341_YELLOW
#define BIRDCOL ILI9341_CYAN
#define PIPECOL ILI9341_GREEN
#define GROUNDCOL ILI9341_BROWN
//
#define speaker 3
#define eon 4
String pr;
// Tạo struct lưu thông tin chim
struct Bird {
  int x, y;
  int vel_y;
};
Bird bird = {50, TFTH / 2, 0};

// Tạo struct lưu thông tin ống
struct Pipe {
  int x, gap_y;
};
Pipe pipe = {TFTW, random(50, TFTH - 50 - GAPHEIGHT)};

bool gameOver = false;
int score = 0;
int highscore = 0; // Lưu điểm cao nhất
unsigned long lastUpdate = 0;
const int frameDelay = 50; // Tốc độ game (ms)
bool check = true;
int count = 0;
void setup() {
  tft.begin(40000000);
  tft.setRotation(1); // Xoay ngang màn hình
  tft.fillScreen(BCKGRDCOL);
  pinMode(2, INPUT_PULLUP); // Nút nhấn nhảy chim
  pinMode(speaker, OUTPUT);
  pinMode(eon, INPUT_PULLUP);
  SetupGame();
}

void SetupGame() {
  gameStartScreen();

  // Khởi tạo trạng thái game
  bird = {50, TFTH / 2, 0};
  pipe = {TFTW, random(50, TFTH - 50 - GAPHEIGHT)};
  gameOver = false;
  score = 0;
}

void loop() {
  if (gameOver) {
    gameOverScreen();
    return;
  }

  if (millis() - lastUpdate >= frameDelay) {
    lastUpdate = millis();
    updateBird();
    updatePipe();
    checkCollision();
    checkbth();
    audio();
  }
}

void updateBird() {
  // Xóa chim cũ
  tft.fillRect(bird.x, bird.y, BIRDW, BIRDH, BCKGRDCOL);

  // Cập nhật vị trí chim
  bird.vel_y += GRAVITY;
  bird.y += bird.vel_y;

  // Chim bay lên khi nhấn nút
  if (digitalRead(2) == LOW) {
    bird.vel_y = FLAP_STRENGTH;
  }

  // Giới hạn chim trong màn hình
  if (bird.y < 0) bird.y = 0;
  if (bird.y > TFTH - BIRDH) bird.y = TFTH - BIRDH;

  // Vẽ chim mới
  tft.fillRect(bird.x, bird.y, BIRDW, BIRDH, BIRDCOL);
}

void updatePipe() {
  // hàm kiểm tra chim qua cột
  tft.fillRect(pipe.x + PIPEW, 15, 5, TFTH, BCKGRDCOL);

  // Cập nhật vị trí ống
  pipe.x -= 5;
  if (pipe.x < -PIPEW) {
    pipe.x = TFTW;
    pipe.gap_y = random(50, TFTH - 50 - GAPHEIGHT);
    score++;

    // Cập nhật highscore nếu đạt điểm cao mới
    if (score > highscore) {
      highscore = score;
    }
  }

  // Vẽ ống mới
  tft.fillRect(pipe.x, 15, PIPEW, pipe.gap_y, PIPECOL);
  tft.fillRect(pipe.x, pipe.gap_y + GAPHEIGHT, PIPEW, TFTH - pipe.gap_y - GAPHEIGHT, PIPECOL);

  // Hiển thị điểm số
  tft.fillRect(0, 0, 240, 15, ILI9341_BLACK);
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print("Score: ");
  tft.print(score);

  // Hiển thị điểm cao nhất
  tft.setCursor(70, 0);
  tft.print("| High Score: ");
  tft.print(highscore);
  //âm thanh
  tft.setCursor(170, 0);
  tft.print("| Audio:");
  tft.print(pr);
}

void checkCollision() {
  // Kiểm tra va chạm ống
  if (bird.x + BIRDW > pipe.x && bird.x < pipe.x + PIPEW) {
    if (bird.y < pipe.gap_y || bird.y + BIRDH > pipe.gap_y + GAPHEIGHT) {
      gameOver = true;
    }
  }

  // Kiểm tra va chạm đất
  if (bird.y >= TFTH - BIRDH) {
    gameOver = true;
  }
}

void gameStartScreen() {
  tft.fillScreen(BCKGRDCOL);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 100);
  tft.println("FLAPPY BIRD");
  tft.setCursor(40, 140);
  tft.println("Press to Start");

  while (digitalRead(2) == HIGH) {
    delay(10);
  }

  tft.fillScreen(BCKGRDCOL);
}

void gameOverScreen() {
  audio_over();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(3);
  tft.setCursor(35, 100);
  tft.println("GAME OVER");
  tft.setTextSize(2);
  tft.setCursor(35, 160);
  tft.println("Score: " + String(score));
  tft.setCursor(30, 75);
  tft.setTextColor(ILI9341_WHITE);
  tft.println("Press to Restart");

  while (digitalRead(2) == HIGH) {
    delay(10);
  }

  SetupGame();
}
bool buttonPressed = false; // Trạng thái nút

void audio() {
  if(check){
    if (digitalRead(2) == HIGH) {
      if (!buttonPressed) { // Kiểm tra nếu nút chưa được bấm
        buttonPressed = true; // Đánh dấu nút đã bấm
        digitalWrite(speaker, HIGH);
        delay(50); // Thời gian phát âm thanh
        digitalWrite(speaker, LOW);
      }
    } else {
      buttonPressed = false; // Reset trạng thái khi nút được thả
      digitalWrite(speaker, LOW);
    }
  }else{
    digitalWrite(speaker, LOW);
  }
}
void audio_over(){
  if(check){
    for(int i = 2500; i >= 1000; i--){
      tone(speaker, i, 500); // Phát tần số
    }
  }else{
    digitalWrite(speaker, LOW);
  }
}
bool lastState = LOW; // Biến để lưu trạng thái trước đó của nút

void checkbth() {
  int currentState = digitalRead(eon); // Đọc trạng thái hiện tại của nút
  
  // Kiểm tra nếu trạng thái của nút thay đổi từ LOW sang HIGH
  if (currentState == HIGH && lastState == LOW) {
    count += 1;
    check = !check; // Đảo ngược giá trị của check
  }

  // Cập nhật trạng thái cuối cùng
  lastState = currentState;
  if(check){
    pr = "on";
  }else{
    pr = "off";
  }
}
//--------------------------------------------------------------------------------