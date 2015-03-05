#define RESTART_PIN 2
#define PAUSE_PIN 3
#define DATA_PIN 5
#define CLOCK_PIN 6
#define ENABLE_PIN 7

//the state variables for different modes:
volatile boolean run_watch = 0;

//and the variables to hold the time:

long ten_second = 0, second = 0, tenth_of_sec = 0, hundredth_of_sec = 0;

// the setup function runs once when you press reset or power the board
void setup() 
{
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(PAUSE_PIN, INPUT);
  pinMode(RESTART_PIN, INPUT);
  
  
  digitalWrite(PAUSE_PIN, HIGH);
  digitalWrite(RESTART_PIN, HIGH);
  
  attachInterrupt(0, set_restart, FALLING);
  attachInterrupt(1, sleep, FALLING);
  //setup the LED driver chip
  setup_chip();
}


void loop() 
{
  count();
}




//ADDITIONAL FUNCTIONS:

void count()
{
  while(run_watch)
  {
    hundredth_of_sec++;
    
    if(hundredth_of_sec >= 10)
    {
      tenth_of_sec++;
      hundredth_of_sec = 0;
      
      if(tenth_of_sec >= 10)
      {
        second++;
        tenth_of_sec = 0;
        
        if(second >= 10)
        {
          ten_second++;
          second = 0;
          
          if(ten_second >= 6)
          {
            ten_second = 0;
          }
        }
      }
    }
      
    send_time();

    delayMicroseconds(10000);
  }
}


void debounce()
{
  if(!digitalRead(PAUSE_PIN))
  {
    delayMicroseconds(10000);
    if(!digitalRead(PAUSE_PIN))
      while(1)
      {
        if(digitalRead(PAUSE_PIN))
        {
           delayMicroseconds(10000);
           if(digitalRead(PAUSE_PIN))
           {
             !run_watch ? run_watch = 1 : run_watch = 0;
             return;
           }
        }
      }
  }
}

void sleep()
{
  debounce();
}

void send_time()
{
  byte first_byte = ten_second + 0x80;
  byte second_byte = (second << 4) + tenth_of_sec;
  byte third_byte = hundredth_of_sec << 4;
  
  digitalWrite(ENABLE_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, first_byte);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, second_byte);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, third_byte);
  digitalWrite(ENABLE_PIN, HIGH);
}

void set_restart()
{
  setup_chip();
  
  ten_second = 0;
  second = 0;
  tenth_of_sec = 0;
  hundredth_of_sec = 0;
  
  run_watch = 0;
}

void setup_chip()
{
  //set the chip to operate in normal mode:
  digitalWrite(ENABLE_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x01);
  digitalWrite(ENABLE_PIN, HIGH);
  delayMicroseconds(10000);
  
  //set all 4 7-segment displays to display 0:
  digitalWrite(ENABLE_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x80);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x00);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x00);
  digitalWrite(ENABLE_PIN, HIGH);
  delayMicroseconds(10000);
}
