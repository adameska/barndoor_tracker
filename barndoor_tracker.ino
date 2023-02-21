
//Input pins
const int InputSelectPin = 2;
const int ActionButton = 4;

//variables for the motor pins
const int motorPin1 = 10;    // Blue   - 28BYJ48 pin 1
const int motorPin2 = 11;    // Pink   - 28BYJ48 pin 2
const int motorPin3 = 12;    // Yellow - 28BYJ48 pin 3
const int motorPin4 = 13;    // Orange - 28BYJ48 pin 4
// Red    - 28BYJ48 pin 5 (VCC)
const int lookup[8] = { B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001 };

int ActionSelector = 0;
bool isRotating = false;
bool isRewinding = false;

const int alterSpeedAmount = 200;

const int sleepTime = 100;

int eartRotationSleep = 4000;  //variable to set stepper speed
int unwindSleep = 900;  //variable to set stepper speed

void setup() {
	pinMode(motorPin1, OUTPUT);
	pinMode(motorPin2, OUTPUT);
	pinMode(motorPin3, OUTPUT);
	pinMode(motorPin4, OUTPUT);

	//when circuit is open pin is high
	pinMode(ActionButton, INPUT_PULLUP);
	pinMode(InputSelectPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

	// initialize serial communication:
	Serial.begin(9600);
}

void loop()
{
	checkInput();

	while (isRotating)
	{
		counterEarthRotation(); //built in sleep so no need for a delay
	}

	while (isRewinding)
	{
		rewindBolt(); //built in sleep so no need for a delay
		checkInput();
	}

	//make sure motor is off if we're not doing anything
	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(motorPin3, LOW);
	digitalWrite(motorPin4, LOW);

	delay(sleepTime);
}

void checkInput() {
  bool buttonPressed = false;

	bool shouldPerformAction = digitalRead(ActionButton) == 0;
	if (shouldPerformAction)
	{
		Serial.print("Detected Action push.\r\n");
		performAction();
    buttonPressed = true;
	}

	bool changingState = digitalRead(InputSelectPin) == 0;
	if (changingState)
	{
		Serial.print("Change State Push.\r\n");
		changeState();
    buttonPressed = true;
	}

  if (buttonPressed) {
    delay(500); //ignore input for a half second    
  }
}

void changeState(){
  ActionSelector++;
  if(ActionSelector == 4){
    ActionSelector = 0;
  }

  for(int i = 0; i <= ActionSelector; i++){
    blink();
  }
}

void blink(){
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(500);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(300);                      // wait for a second
}

void performAction(){
  if (ActionSelector == 0) {
    isRotating = !isRotating;
		isRewinding = false;
  } 
  else if (ActionSelector == 1) {
    isRewinding = !isRewinding;
		isRotating = false;    
  }
  else if (ActionSelector == 2) {
    eartRotationSleep -= alterSpeedAmount;
    Serial.print("Rotate Speed: ");
    Serial.print(eartRotationSleep);
    Serial.print("\r\n");
  }
  else if (ActionSelector == 3) {
    eartRotationSleep += alterSpeedAmount;
    Serial.print("Rotate Speed: ");
    Serial.print(eartRotationSleep);
    Serial.print("\r\n");
  }
}

void counterEarthRotation(){
	for (int i = 0; i < 8; i++)
	{
		setOutput(i);
		delayMicroseconds(eartRotationSleep);
		checkInput();
	}
}

void rewindBolt(){
  for (int i = 7; i >= 0; i--)
	{
		setOutput(i);
		delayMicroseconds(unwindSleep);
		checkInput();
	}
}

void setOutput(int out)
{
	digitalWrite(motorPin1, bitRead(lookup[out], 0));
	digitalWrite(motorPin2, bitRead(lookup[out], 1));
	digitalWrite(motorPin3, bitRead(lookup[out], 2));
	digitalWrite(motorPin4, bitRead(lookup[out], 3));
}
