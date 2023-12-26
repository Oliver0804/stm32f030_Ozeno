// 引腳定義
#include <Arduino.h>


const int buttonPin = PIN_A3;  // PA3替換成實際的Arduino引腳號
const int led1Pin = PIN_A4;    // PA4替換成實際的Arduino引腳號
const int led2Pin = PIN_A5;    // PA5替換成實際的Arduino引腳號
const int led3Pin = PIN_A6;    // PA6替換成實際的Arduino引腳號
const int fanPin = PIN_A7;     // PA7替換成實際的Arduino引腳號
const int ozonePin = PIN_A8;   // PA8替換成實際的Arduino引腳號
const int interruptPin = PA14; // PA14替換成實際的Arduino引腳號

// 時間參數
unsigned long modeDurations[3] = {2 * 60 * 60 * 1000, 8 * 60 * 60 * 1000, 16 * 60 * 60 * 1000}; // 模式持續時間
unsigned long ozoneDuration = 30 * 60 * 1000; // 臭氧持續時間

// 其他變數
bool modeActive = false; // 目前模式是否正在執行
unsigned long modeStartTime; // 目前模式開始的時間
int mode = 0; // 目前的模式


// 改變模式的函數
void changeMode() {
  modeActive = false; // 停止當前模式
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);
  digitalWrite(ozonePin, LOW);
  
  mode = (mode + 1) % 3; // 切換到下一個模式
  // 根據模式點亮LED
  if (mode == 0) {
    digitalWrite(led1Pin, HIGH);
  } else if (mode == 1) {
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
  } else if (mode == 2) {
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led3Pin, HIGH);
  }
  
  modeActive = true;
  modeStartTime = millis(); // 記錄模式開始的時間
}

// 啟動臭氧的函數
void startOzone() {
  digitalWrite(ozonePin, HIGH);
  delay(ozoneDuration);
  digitalWrite(ozonePin, LOW);
  modeActive = false; // 臭氧完成，停止模式
}

// 只啟動臭氧，LED閃爍的函數
void startOzoneOnly() {
  for (int i = 0; i < 30; i++) {
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led3Pin, HIGH);
    delay(500);
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
    digitalWrite(led3Pin, LOW);
    delay(500);
  }
  startOzone();
}

// 停止所有運作的函數
void stopProcess() {
  modeActive = false;
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);
  digitalWrite(fanPin, LOW);
  digitalWrite(ozonePin, LOW);
}

void setup() {
  pinMode(PA4, OUTPUT);
    while(1){
    digitalWrite(PA4, HIGH);
    delay(100);
    digitalWrite(PA4, LOW);
    delay(100);
  }
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(PA4, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(ozonePin, OUTPUT);

  //attachInterrupt(digitalPinToInterrupt(interruptPin), stopProcess, FALLING); // 設定外部中斷
}

void loop() {
  static unsigned long lastButtonPressTime = 0; // 上次按鈕按下的時間
  static bool buttonPreviouslyPressed = false;  // 上次按鈕是否被按下

  // 讀取按鈕狀態
  bool buttonPressed = digitalRead(buttonPin) == LOW;
  if (buttonPressed && !buttonPreviouslyPressed) {
    lastButtonPressTime = millis();
    buttonPreviouslyPressed = true;
  } else if (!buttonPressed && buttonPreviouslyPressed) {
    // 按鈕被釋放，檢查短按或長按
    if (millis() - lastButtonPressTime < 1000) { // 短按
      changeMode();
    } else { // 長按
      startOzoneOnly();
    }
    buttonPreviouslyPressed = false;
  }
  
  if (modeActive) {
    // 檢查模式是否完成
    if (millis() - modeStartTime > modeDurations[mode]) {
      // 模式完成，啟動臭氧
      startOzone();
    }
  }
}
