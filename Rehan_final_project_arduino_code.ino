/*----------declaring pin numbers-----------*/
const unsigned char keypadRow1=13,keypadRow2=12,keypadRow3=11,keypadRow4=10;
const unsigned char keypadColumn1=9,keypadColumn2=8,keypadColumn3=7,keypadColumn4=6;

const unsigned char lcdEn=A5,lcdRs=A4,lcdD4=A3,lcdD5=A2,lcdD6=A1,lcdD7=A0;

const unsigned char selectAndJumpButton=3,buzzerPin=4,menuButton=2;
/*------------------------------------------*/

#include<LiquidCrystal.h>
#include<Keypad.h>

char keys[4][4]={
  {'7','8','9','/'},
  {'4','5','6','x'},
  {'1','2','3','-'},
  {'c','0','=','+'}
};
char rows[]={keypadRow1,keypadRow2,keypadRow3,keypadRow4};
char columns[]={keypadColumn1,keypadColumn2,keypadColumn3,keypadColumn4};

Keypad keypad=Keypad(makeKeymap(keys),rows,columns,4,4);

LiquidCrystal lcd(lcdEn,lcdRs,lcdD4,lcdD5,lcdD6,lcdD7);

char keypadInput=0;
char numbers[]={'0','1','2','3','4','5','6','7','8','9'};
char operators[]={'/','x','-','+','=','c'};
volatile bool selectStatus=1;
volatile bool menuStatus=1;

/*-----------------custom characters-------------------*/

byte Speaker[8] = { /*custom char for speaker*/
0b00001,
0b00011,
0b01111,
0b01111,
0b01111,
0b00011,
0b00001,
0b00000
};

byte dino[8] = {
  0b00000,
  0b00111,
  0b00111,
  0b10110,
  0b11111,
  0b01010,
  0b01010,
  0b00000
};

byte block[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};
/*-----------------------------------------------------*/

void setup() {
  lcd.begin(20,4);
  lcd.createChar(1, Speaker);
  lcd.createChar(2, block);
  lcd.createChar(3, dino);
  pinMode(buzzerPin,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(selectAndJumpButton),selectISR,RISING); /*hardware interrupt for selcting options*/
  attachInterrupt(digitalPinToInterrupt(menuButton),menuISR,RISING); /*hardware interrupt for going to menu page*/
  Serial.begin(9600);
}

void selectISR()
{
 selectStatus=!selectStatus; /*toggle selectStatus whenever interrupt happens*/ 
}

void menuISR()
{
  menuStatus=!menuStatus; /*toggle menuStatus whenever interrupt happens*/
}

void displayTaskbar()
{
  lcd.setCursor(0,3);
  lcd.print("Select");
  lcd.setCursor(16,3);
  lcd.print("Menu");
}

char checkNum(char n){ /*return char num if the inputdata is a number otherwise return 0*/
  for(char i=0;i<=9;i++)
  {
    if(numbers[i]==n) 
    {
      return numbers[i];
    }
  } 
  return 0;
}

char checkOperator() /*return operator sign or else 0*/
{
  for(char i=0;i<=5;i++)
  {
    if(keypadInput==operators[i])
    {
      return operators[i];
    }
    else
    {
    }
  }
  return 0;
}

/*----------------------Menu code start--------------------------*/
void menu()
{
 menuStatus=0;
 lcd.clear();
 lcd.setCursor(8,0);
 lcd.print("Menu");
 lcd.setCursor(0,1);
 lcd.print("1) Clock");
 lcd.setCursor(0,2);
 lcd.print("2) Calculator");
 lcd.setCursor(0,3);
 lcd.print("Press keypad key");
 takeMenuInputs();
}

void takeMenuInputs()
{
  keypadInput=keypad.waitForKey();
  if(keypadInput=='1')
  {
   Clock();
  }
  else if(keypadInput=='2')
  {
   calculator();
  }
  else
  {
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("Invalid Entry");
    delay(500);
  }
}
/*----------------------Menu code end--------------------------*/

/*----------------------Clock code start-----------------------*/
unsigned char seconds=0,minutes=0,hours=0;
unsigned long millisOut=0,millisOutDelay=0;
char alarmTime[2]; /*this array will store time entered from keypad*/
unsigned char alarmSeconds=0,alarmMinutes=0,alarmHours=0;
bool alarmSet=0; /*this will check if alarm time is set or not*/
unsigned char invalidRow=0;

void Clock()
{
  selectStatus=0;
  menuStatus=0;
  lcd.clear();
  lcd.setCursor(7,0);
  lcd.print("Clock");
  lcd.setCursor(1,2);
  lcd.print("Set Alarm (Select)");
  lcd.setCursor(0,3);
  displayTaskbar();
  while(menuStatus==0) /*displayCurrentTime untill interrupt happens*/
  {
   if(selectStatus==0)
   {
    lcd.setCursor(4,1);
    displayCurrentTime();
   }
   else
   {
    displaySetAlarm(); /*this function takes and stores alarm time input from the user and display it to LCD*/
   }
   checkAlarmTime(); /*this will check if current time == alarm time and turns on the buzzer for 10 sec*/
  }
}

void checkAlarmTime() /*this will check if current time == alarm time and turns on the buzzer for 10 sec*/
{
  if((not(seconds==0 and minutes==0 and hours==0)) and /*this will prevent alarm if sec,min,hr all are 0
                                                         i.e in starting time*/
     seconds==alarmSeconds and minutes==alarmMinutes and hours==alarmHours and millis()>1000)
  {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Alarm is ON!!!");
    millisOutDelay=millis()/1000;
    while(1)
    {
    if(millisOutDelay+10<=(millis()/1000)) /*break the loop after 10 seconds*/
    { 
     break;
    } 
    else
    {
      buzzerOn(); /*buzzer on for 500 milli seconds*/
      delay(500);
      buzzerOff(); /*buzzer off for 500 milli seconds*/
      delay(500);
    }
    }
    lcd.clear();
    lcd.setCursor(7,0);
    lcd.print("Clock");
    lcd.setCursor(1,2);
    lcd.print("Set Alarm (Select)");
    lcd.setCursor(0,3);
    displayTaskbar(); 
  }
  else
  { 
  }
}

void buzzerOn()
{
  lcd.setCursor(7,2); /*printing custom char of buzzer*/
  lcd.write(byte(1));
  lcd.print(" ");
  lcd.write(byte(1));
  lcd.print(" "); 
  lcd.write(byte(1));
  lcd.print(" ");
  tone(buzzerPin,1000);
}

void buzzerOff()
{
  lcd.setCursor(7,2);
  lcd.print("            ");/*erasing custom char of buzzer*/
  lcd.setCursor(7,2);
  noTone(buzzerPin);
}

void displaySetAlarm()
{
  selectStatus=0;
  alarmSet=1;
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Enter Alarm Time");
  lcd.setCursor(0,1);
  lcd.print("Hours   : ");
  takeTimeInput(); /*taking hours time*/
  alarmHours=extractTimeFromArray();
  while(alarmHours>=24) /*hours should be less than 24*/
  {
    invalidRow=1;
    printInvalid();
    takeTimeInput();
    alarmHours=extractTimeFromArray();
  }
  lcd.setCursor(0,2);
  lcd.print("Minutes : ");
  takeTimeInput(); /*taking minutes time*/
  alarmMinutes=extractTimeFromArray();
  while(alarmMinutes>=60) /*minutes should be less than 60*/
  {
    invalidRow=2;
    printInvalid();
    takeTimeInput();
    alarmMinutes=extractTimeFromArray();
  }
  lcd.setCursor(0,3);
  lcd.print("Seconds : ");
  takeTimeInput(); /*taking seconds time*/
  alarmSeconds=extractTimeFromArray();
  while(alarmSeconds>=60) /*seconds should be less than 60*/
  {
    invalidRow=3;
    printInvalid();
    takeTimeInput();
    alarmSeconds=extractTimeFromArray();
  }
  delay(1000);
  displaySetTime();
}

void displaySetTime()
{
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Alarm Set at");
  lcd.setCursor(4,2);
  if(alarmHours<=9) /*printing hours minutes and seconds of alarm set time*/
  {
   lcd.print("0");
   lcd.print(alarmHours); 
  }
  else
  {
   lcd.print(alarmHours);
  }
  lcd.print(" : ");
  if(alarmMinutes<=9)
  {
   lcd.print("0");
   lcd.print(alarmMinutes);
  }
  else
  {
   lcd.print(alarmMinutes);  
  }
  lcd.print(" : ");
  if(alarmSeconds<=9)
  {
   lcd.print("0");
   lcd.print(alarmSeconds);
  }
  else
  {
   lcd.print(alarmSeconds);
  } 
  delay(2000);
  lcd.clear();
  lcd.setCursor(7,0);
  lcd.print("Clock");
  lcd.setCursor(1,2);
  lcd.print("Set Alarm (Select)");
  lcd.setCursor(0,3);
  displayTaskbar();
}
void takeTimeInput()
{
  keypadInput=keypad.waitForKey();
  while(checkNum(keypadInput)==0) /*checking first digit of time*/
  {
    printInvalid();
    keypadInput=keypad.waitForKey();
  }
  lcd.print(keypadInput);
  alarmTime[0]=keypadInput;
  keypadInput=keypad.waitForKey();
  while(checkNum(keypadInput)==0) /*checking second digit of time*/
  {
    printInvalid();
    keypadInput=keypad.waitForKey();
  }
  lcd.print(keypadInput);
  alarmTime[1]=keypadInput;
}

unsigned char extractTimeFromArray()
{
  unsigned char firstDigit=alarmTime[0]-'0';
  unsigned char secondDigit=alarmTime[1]-'0';
  return (firstDigit*10)+secondDigit;
}

void printInvalid()
{
  lcd.setCursor(10,invalidRow);
  lcd.print("Invalid");
  delay(500);
  lcd.setCursor(10,invalidRow);
  lcd.print("       "); /*erasing invalid*/
  lcd.setCursor(10,invalidRow);
}

void displayCurrentTime() /*clock will start when arduino is powered on*/
{
  seconds=millis()/1000; /*getting sec val from millis()*/
  minutes=seconds/60;
  hours=minutes/60;
  seconds=seconds%60; /*sec will repeat after 59*/
  if(hours<=9) /*printing hours minutes and seconds*/
  {
   lcd.print("0");
   lcd.print(hours); 
  }
  else
  {
   lcd.print(hours);
  }
  lcd.print(" : ");
  if(minutes<=9)
  {
   lcd.print("0");
   lcd.print(minutes);
  }
  else
  {
   lcd.print(minutes);  
  }
  lcd.print(" : ");
  if(seconds<=9)
  {
   lcd.print("0");
   lcd.print(seconds);
  }
  else
  {
   lcd.print(seconds);
  }
} 
/*----------------------Clock code end-----------------------*/

/*----------------------Calculator code start---------------------*/
unsigned int firstNum=0,secondNum=0;
float result=0;
char currentOperator=0,previousOperator=0;

char inputArray[21]; /*max. length of input 21*/
char inputArrayStartIndex=0,inputArrayEndIndex=-1;
char numLength=0;

float performCalculation(float num1,unsigned int num2,char operat)
{
  if(operat=='+')
  {
    return num1+num2;
  }
  else if(operat=='-')
  {
    return num1-num2;
  }
  else if(operat=='x')
  {
    return num1*num2;
  }
  else if(operat=='/')
  {
    return num1/num2;
  }
}

unsigned int combiningNumFromArray()
{
  unsigned int num=0,multiplier=1; /*num can be 65535 max.*/
  char i=inputArrayStartIndex;
  numLength=0;
  while(1) /*extracting length of first num from array*/
  {
    if(checkNum(inputArray[i])!=0) /*operator is not there*/
    {
      numLength++;
    }
    else
    {
      break;
    }
    i++;
  }
  for(i=inputArrayStartIndex+numLength-1;i>=inputArrayStartIndex;i--) /*extracting number using numLength*/
  {
    num+=(inputArray[i]-'0')*multiplier;
    multiplier*=10;
  }
  return num;
}

void performCalculationsOnArray()
{
  result=0;
  previousOperator=0;
  while(1)
  {
    firstNum=combiningNumFromArray();/*storing first number*/
    if(previousOperator==0) /*doing calculation on first two number*/
    {
     currentOperator=inputArray[inputArrayStartIndex+numLength]; /*operator is at last*/ 
     if(currentOperator=='=')
     {
      lcd.setCursor(0,2);
      lcd.print("Ans : ");
      lcd.print(firstNum);
      lcd.setCursor(0,3);
      lcd.print("Press keypad key");
      break;
     }
     else
     {
     inputArrayStartIndex+=(numLength+1); /*updating the value of inputArrayStartIndex*/
     secondNum=combiningNumFromArray();/*storing second number*/
     result=performCalculation(firstNum,secondNum,currentOperator); /*updating result*/
     previousOperator=inputArray[inputArrayStartIndex+numLength]; 
     inputArrayStartIndex+=(numLength+1); /*updating the value of inputArrayStartIndex*/
     }  
     if(previousOperator=='=') /*stop the calculation if '=' is encountered in array*/
     {
      lcd.setCursor(0,2);
      lcd.print("Ans : ");
      lcd.print(result,3);
      lcd.setCursor(0,3);
      lcd.print("Press keypad key");
      break;
     }
     else
     {
     }
    }
    else
    {
     result=performCalculation(result,firstNum,previousOperator); /*updating result*/
     previousOperator=inputArray[inputArrayStartIndex+numLength]; /*operator is at last*/ 
     inputArrayStartIndex+=(numLength+1); /*updating the value of inputArrayStartIndex*/
     if(previousOperator=='=') /*stop the calculation if '=' is encountered in array*/
     {
      lcd.setCursor(0,2);
      lcd.print("Ans : ");
      lcd.print(result,3);
      lcd.setCursor(0,3);
      lcd.print("Press keypad key");
      break;
     }
     else
     {
     }
    }
  }
  keypadInput=keypad.waitForKey();
}

void takeCalculatorInputs()
{
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Calculator");
  lcd.setCursor(0,1);
  if(checkNum(keypadInput)==0) /*if first entry is not a number*/
  {
    lcd.setCursor(3,1);
    lcd.print("Invalid Entry");
    delay(500);
    return; 
  }
  else
  {
  while(1) /*take numbers and operators untill untill '=' is not pressed*/
  {
    if(keypadInput=='c') /*delete the previous entry*/
    {
      lcd.setCursor(inputArrayEndIndex,1);
      lcd.print(" ");
      lcd.setCursor(inputArrayEndIndex,1);
      if(inputArrayEndIndex==-1)
      {
        lcd.setCursor(0,1);
        inputArrayEndIndex=-1;
      }
      else
      {
        inputArrayEndIndex--;
      }
    }
    else if(keypadInput!='=') /*'=' is not pressed, then add it to array*/
    {
      inputArrayEndIndex++;
      inputArray[inputArrayEndIndex]=keypadInput; /*store the number in array*/
      lcd.print(keypadInput);
    }
    else /*stop taking input if '=' is pressed*/
    {
      inputArrayEndIndex++;
      inputArray[inputArrayEndIndex]=keypadInput; /*store the '=' in array*/
      break;
    }
    keypadInput=keypad.waitForKey();
  }
  }
  performCalculationsOnArray(); /*get result from array*/
}

void calculator()
{
  while(1) /*calcution will be on untill menu is not pressed*/
  {
   bool inCalculator=1;
   menuStatus=0;
   lcd.clear();
   lcd.setCursor(5,0);
   lcd.print("Calculator");
   displayTaskbar();
   lcd.setCursor(4,1);
   lcd.print("Enter number");
   keypadInput=keypad.getKey();
   while(not(keypadInput>0)) /*key is not pressed*/
   {
    if(menuStatus==1) /*if menu is pressed then go to menu page*/
    {
      inCalculator=0;
      break;
    }
    else
    {
      keypadInput=keypad.getKey();
    } 
   }
   if(inCalculator==0)
   {
    break;
   }
   else
   {
    takeCalculatorInputs();    
   }
  }
}
/*----------------------Calculator code end---------------------*/  
  
void loop() 
{
  menu(); /*menu will be called after every function call is over*/
}
