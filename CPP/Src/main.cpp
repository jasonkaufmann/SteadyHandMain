#include "main.h"
#include "LCDController.h"
#include <cmath>
#include <string>

/******************Global variables****************/
const int testDuration = 10; //in seconds
const int sampleFrequency = 25; //in Hertz (can't go above like 30Hz or we run out of RAM)
const int numSamples = testDuration * sampleFrequency;
const int numRings = 4;
int sensorValues[numSamples][numRings] = {0}; //2D array to record all samples
int calibrationForEachRing[numRings] = {0};
uint32_t period = 1000/sampleFrequency; //This is the amount of time, in ms, to wait before doing next sample.

uint32_t analogReadPins[4] = {ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3};
int w[4] = {4, 3, 2, 1}; //Sensor weights, by ring
int norm = 10; //Normalizing factor to divide initial analogRead values.

float timeRemaining = 0; //Initializing time remaining variable
int countUp = 0;  //Keeps track of iteration count
int calibration = 0; //Initializing calibration variable
int isReady = 0; //Initializing variable to know when the user first shines laser at target to begin test.
/**************************************************************************/


/******** Initializing all functions ********/
uint32_t readADC(ADC_HandleTypeDef gADC, uint32_t channel);
void startUpLCD(LCDController myLCD);
void calibrateSensors();
void waitForLaser();
void runTest(LCDController myLCD);
int calculateScore();
int map(int x, int in_min, int in_max, int out_min, int out_max);
int numDigits(int x) ;
/*********************************/

void CppMain() {

	LCDController myLCD(GPIOB, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13,
	                GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_10, GPIO_PIN_9,
	                GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5);

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4, GPIO_PIN_SET); //keep alive. Do not write low or it will turn off

    startUpLCD(myLCD); //turn on LCD, clear the screen, and home it

    calibrateSensors(); //get the current room illumination as a baseline

    myLCD.clear();
    myLCD.setCursor(7,0);
    myLCD.print("READY");
    myLCD.setCursor(4,2);
    myLCD.print("Aim at Target");
    myLCD.setCursor(3,3);
    myLCD.print("for 10 seconds");

    waitForLaser(); //wait until the laser hits one of the rings to start the test

    myLCD.clear();
    myLCD.setCursor(0,0);
    myLCD.print("HOLD STEADY");

    runTest(myLCD);

    int scorePercentage = calculateScore();

    myLCD.clear();
    myLCD.setCursor(0,0);
    myLCD.print("Score: ");
    myLCD.setCursor(0,3);
    myLCD.print(std::to_string(scorePercentage));
    int digits = numDigits(scorePercentage);
    myLCD.setCursor(digits,3);
    myLCD.print("%");

    HAL_Delay(5000); //wait for 5 seconds for doctor to record score
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4, GPIO_PIN_RESET); //auto-shutoff
}

void runTest(LCDController myLCD) {

    myLCD.setCursor(2,2);
    myLCD.print(" seconds remaining");

    uint32_t start = HAL_GetTick();
    uint32_t countDown, codeRunTime, delayTime; int previousValue = -1; int greatestDiff, ringChoice;
    for (int j = 0; j <= numSamples; j++) { //For each sample (moving through time)
        countDown = HAL_GetTick();
        timeRemaining = testDuration - (float)(countDown-start)/1000.0f; //Time remaining in seconds
        greatestDiff = 0; ringChoice = 0;
        for (int i = 0; i <= (numRings-1); i++) { //For each sensor
        	int analogValue = (int)((readADC(hadc,analogReadPins[i]) / norm) - calibrationForEachRing[i]); //Reading and recording sensor value
        	if (analogValue > greatestDiff) {
                greatestDiff = analogValue;
                ringChoice = i;
            }
        }
        for (int i = 0; i <= (numRings-1); i++) {
        	sensorValues[j][i] = 0;
			if(greatestDiff > 50 && ringChoice == i) {
				sensorValues[j][i] = 1;
			}
        }

        /*** print out the time remaining if the test is not done ***/
        if  (timeRemaining > 0) {
            int roundTime = std::lround(timeRemaining);
            if (roundTime != previousValue) {
                myLCD.setCursor(0, 2);
                myLCD.print("  ");
                myLCD.setCursor(0, 2);
                myLCD.print(std::to_string(roundTime));
            }
            previousValue = roundTime;
        }
        
        codeRunTime = HAL_GetTick();

        //adjust the delay for the next sample depending on the time it took the
        //above code to run
        if ((codeRunTime-countDown) > period) {
        	return;
        } else {
        	 delayTime = period-(codeRunTime-countDown);
        	 HAL_Delay(delayTime); //Wait for the period to sample again
        }
    }
}

int calculateScore() {
	int score = 0; //Initializing score variable
    for (int j = 0; j <= numSamples; j++) { //For each sample (moving through time)
        for (int i = 0; i <= (numRings-1); i++) { //For each sensor
            score = score + w[i]*sensorValues[j][i]; //muliple by appropriate weighting factor for each ring
        }
    }
    return map(score,0,1000,0,100); //map the score from 0 to 100%
}


void waitForLaser() {
    while (isReady < 1.25*calibration) { //While the average reading from all sensors is less than 125% of the calibration read
        isReady = 0; //Sets variable back to zero each time through the while loop
        for (int i = 0; i <= (numRings-1); i++) { // For each sensor
            int value = (int)(readADC(hadc, analogReadPins[i])) / norm;//Read each sensor
            HAL_Delay(5); //Wait 5 ms
            isReady = isReady + (value / numRings); //Summing up all readings and taking average
        }
        countUp++;
        if (countUp > 500) {
            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4, GPIO_PIN_RESET); //turn off if accidentally turned on
        }
    }
}

void calibrateSensors() {

    HAL_Delay(500); //Wait half a second
    int value;
    for (int j = 1; j <= 10; j++) {
    	calibration = 0;
		for (int i = 0; i <= (numRings-1); i++) { // For each sensor
			 value = (int)((readADC(hadc, analogReadPins[i])) / norm); //Read each sensor
			 calibrationForEachRing[i] = ((j-1) * calibrationForEachRing[i] + value)/j;
			 calibration = calibration + (value / numRings);
			HAL_Delay(5); //Wait 5 ms
		}
    }
}

void startUpLCD(LCDController myLCD) {
    myLCD.init(); //initiate the LCD
    myLCD.clear(); //get rid of any remaining text on the screen
    myLCD.home(); //set the cursor to home
}


uint32_t readADC(ADC_HandleTypeDef gADC, uint32_t channel) {
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel = channel;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&gADC, &sConfig);

	HAL_ADCEx_Calibration_Start(&gADC, ADC_SINGLE_ENDED);

	HAL_ADC_Start(&gADC);
	HAL_ADC_PollForConversion(&gADC, HAL_MAX_DELAY);
	uint32_t raw = HAL_ADC_GetValue(&gADC);
	HAL_ADC_Stop(&gADC);

	sConfig.Rank = ADC_RANK_NONE;
	HAL_ADC_ConfigChannel(&gADC, &sConfig);
	return raw;
}

int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int numDigits(int x)
{
    return (x < 10 ? 1 : (x < 100 ? 2 : 3));
}
