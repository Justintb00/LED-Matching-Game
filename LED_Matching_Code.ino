/*
	LED Matching Game by: Justin Bennett
    GitHub @justintb00
    
    The purpose of this game is to match the pattern correctly.
    If you fail to do so, you will have to restart. The higher the
    score, the more difficult it will get. There are various factors
    such as a faster blink time through progression and
*/
#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal Screen(A5, A4, A3, A2, A1, A0);

/* These are the main pins used to setup*/
int btn_yellow = 2;
int btn_red = 3;
int btn_orange = 4;
int btn_green = 5;
int yellow_led = 6;
int red_led = 7;
int orange_led = 8;
int green_led = 9;
int lights []= {yellow_led, red_led, orange_led, green_led}; // Array used to coordinate the buttons to the leds using hashing

int Gamemode = 0; // 0 = Start, 1 = Pattern In Progress, 2 = Game in Progress, 3 = W/L
int HighScore = 0; //High Score within each session
int CurrentScore = 0;
int time = 30; // Will keep the main time
int CurrentTime = time; // The time we'll be iterating from
int numBlinks = 3;// Number of blinks that will occur in a sequence

int Pattern[16]; // Will hold the current pattern
int pat_iter = 0; // Iterates through pattern

void setup()
{
  for(int i = 0; i < (sizeof(lights) / sizeof(lights[0])); i++)
  {
    pinMode(lights[i], OUTPUT);
  }
  pinMode(btn_green, INPUT_PULLUP);
  pinMode(btn_red, INPUT_PULLUP);
  pinMode(btn_orange, INPUT_PULLUP);
  pinMode(btn_yellow, INPUT_PULLUP);
  Screen.begin(16,2);
  Screen.print("Hello World");
  Serial.begin(9600);
}
void GameStart()
{
  Screen.setCursor(0,0);
  Screen.print("Please press the green button to start!! ");
  Screen.scrollDisplayLeft();
  delay(75);
  Screen.setCursor(1,1);
  Screen.print("Current High: ");
  Screen.print(HighScore);
}

void Blink(int LED, int time){
  digitalWrite(LED, HIGH);
  delay(time);
  digitalWrite(LED, LOW);
  delay(50);
}

void clearPattern(int ary []){
  int i = 0;
  while (ary[i] != 0){
    ary[i] = 0;
    i++;
  }
}
 
void GameOver(){
  //flashAllLights();
  Screen.clear();
  Screen.setCursor(0,0);
  Screen.print("Game Over");
  Screen.setCursor(7,1);
  Screen.print("Score: ");
  Screen.print(CurrentScore);
  if (CurrentScore > HighScore){
    HighScore = CurrentScore;
  }
  clearPattern(Pattern);
  CurrentScore = 0;
  pat_iter = 0;
  time = 30;
  CurrentTime = time;
  numBlinks = 3;
  Gamemode = 0;
  delay(3000);
}

void Success(int time){
  int i = 5;
  while(i > 0){
    Screen.print("Congratulations!!!");
    Screen.setCursor(3,1);
    Screen.print("Wait: ");
    Screen.print(i);
    delay(800);
    Screen.clear();
    i--;
  }
  CurrentScore++;
  if (CurrentScore % 10 == 0){
    time -= 5;
  }
  CurrentTime = time;
  pat_iter = 0;
  if (numBlinks < 16){
    numBlinks++;
  }
  Gamemode = 1;
  
}

int input(int time)
{
  while(time > 0){
    if (digitalRead(btn_green) == !HIGH){
      return btn_green;
    }
    if (digitalRead(btn_orange) == !HIGH){
      return btn_orange;
    }
    if (digitalRead(btn_yellow) == !HIGH){
      return btn_yellow;
    }
    if (digitalRead(btn_red) == !HIGH){
      return btn_red;
    }
    time--;
    delay(1);
  }
  return 0;
}
/*
void flashAllLights(){
  int j = 0;
  while (j < 6){
    for (int i = 0; i < (sizeof(lights) / sizeof(lights[0])); i++)
    {
      digitalWrite(i, HIGH);
    }
    delay(130);
    for (int i = 0; i < (sizeof(lights)/sizeof(lights[0])); i++){
      digitalWrite(i, LOW);
    }
    delay(20);
    j++;
  }
}
*/

void loop()
{
  if (Gamemode == 0){
    GameStart();
    if (input(1) == 5){
      Gamemode = 1;
      Screen.clear();
    }
  }
  else if (Gamemode == 1){
    Screen.print("Thinking!!!");
    for(int i = 0; i < numBlinks; i++){
      int rand = random(4);
      int LED = lights[rand];
      Pattern[i] = lights[rand];
      int blinkTime = 700;
      if (CurrentScore % 3 == 0){
        int difference = 50 * (CurrentScore / 3);
        if (difference > 450){
          blinkTime = 250;
        }
        else{
        	blinkTime -= (50 * (CurrentScore / 3));
        }
      }
      Blink(LED, blinkTime);
    }
    Screen.clear();
    Gamemode = 2;
    delay(100);
  }
  else if (Gamemode == 2){
    for(int i = numBlinks - pat_iter; i > 0; i--)
    {
      Screen.print("#");
    }
    
    Screen.setCursor(8,1);
    Screen.print("Time: ");
    Screen.print(CurrentTime);
    
    if (Pattern[pat_iter] == 0){
      Serial.println("+1 Congrats");
      Screen.clear();
      Success(time);
    }
    
    else if (CurrentTime == 0){
      Serial.println("Game Over!!!!");
      GameOver();
    }
    
    int val = input(800);
    if (val != 0){
      int hashedBtn = lights[(val + 2) % 4];
      digitalWrite(hashedBtn, HIGH); // Turns on Coorsponding Light
      Serial.println(hashedBtn);
      if (hashedBtn != Pattern[pat_iter]){
        digitalWrite(hashedBtn, LOW);
        Serial.println("Wrong LED, Game Over!!!!");
        GameOver();
      }
      else{
        pat_iter++;
        Serial.println("Correct");
      }
    }
    CurrentTime--;
    delay(200);
    digitalWrite(lights[(val + 2) % 4], LOW);
    Screen.clear();
  }
}