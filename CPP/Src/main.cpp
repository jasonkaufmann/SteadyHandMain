#include "main.h"
#include "LCDController.h"
#include <string>
#include "string.h"

/******************Global variables****************/
const int testDuration = 10; //in seconds
const int sampleFrequency = 25; //in Hertz
const int numSamples = testDuration * sampleFrequency;
const int numRings = 4;
int sensorValues[numSamples][numRings] = {}; //2D array to record all samples
int period = 1000/sampleFrequency; //This is the amount of time, in ms, to wait before doing next sample.

uint32_t analogReadPins[4] = {ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3};
double w[4] = {2, 1.5, 1.25, 1}; //Sensor weights, by ring
int max_score = 1; //Normalizing factor to divide by to make score a percent from 1% to 100%
int norm = 10; //Normalizing factor to divide initial analogRead values.

int timeRemaining = 0; //Initializing time remaining variable
int countUp = 0;  //Keeps track of iteration count
int calibration = 0; //Initializing calibration variable
int isReady = 0; //Initializing variable to know when the user first shines laser at target to begin test.

int score = 0; //Initializing score variable
/**************************************************************************/


/******** Initializing all functions ********/
uint32_t readADC(ADC_HandleTypeDef hadc, uint32_t channel);
void startUpLCD(LCDController);
void calibrateSensors();
void waitForLaser();
void runTest(LCDController myLCD);
void calculateScore();
/*********************************/

void CppMain() {

	LCDController myLCD(GPIOB, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13,
	                GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_10, GPIO_PIN_9,
	                GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5);

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4, GPIO_PIN_SET); //keep alive. Do not write low or it will turn off

    startUpLCD(myLCD);

    calibrateSensors();

    myLCD.clear();
    myLCD.setCursor(7,0);
    myLCD.print("READY");
    myLCD.setCursor(4,2);
    myLCD.print("Aim at Target");
    myLCD.setCursor(3,3);
    myLCD.print("for 10 seconds");

    waitForLaser();

    myLCD.clear();
    myLCD.setCursor(0,0);
    myLCD.print("HOLD STEADY");

    runTest(myLCD);

    calculateScore();

    myLCD.clear();
    myLCD.setCursor(0,0);
    myLCD.print("Score: ");
    myLCD.setCursor(0,3);
    myLCD.print(std::to_string(score));

    HAL_Delay(5000); //wait for 5 seconds for doctor to record score
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4, GPIO_PIN_RESET); //auto-shutoff
}

void runTest(LCDController myLCD) {
    for (int j = 0; j <= numSamples; j++) { //For each sample (moving through time)
        timeRemaining = testDuration - ((period*(j+1))/1000); //Time remaining in seconds
        for (int i = 0; i <= (numRings-1); i++) { //For each sensor
            sensorValues[j][i] = (int)((readADC(hadc,analogReadPins[i]) / norm) - calibration); //Reading and recording sensor value
            //HAL_Delay(3); //Wait 3 ms in between sensor readings
        }
        if  (timeRemaining > 0) {

            myLCD.setCursor(0,2);
            myLCD.print("  ");
            myLCD.setCursor(0,2);
            myLCD.print(std::to_string(timeRemaining));
            myLCD.setCursor(2,2);
            myLCD.print(" seconds remaining");
        }
        HAL_Delay(period); //Wait
    }
}

void calculateScore() {
    for (int j = 0; j <= numSamples; j++) { //For each sample (moving through time)
        for (int i = 0; i <= (numRings-1); i++) { //For each sensor
            score = score + w[i]*sensorValues[j][i]; //muliple by appropriate weighting factor for each ring
        }
    }
    score = score/max_score; //Normalizing score to make it a percent from 1% to 100%
}


void waitForLaser() {
    while (isReady < 1.01*calibration) { //While the average reading from all sensors is less than 110% of the calibration read
        isReady = 0; //Sets variable back to zero each time through the while loop
        score = 0;
        for (int i = 0; i <= (numRings-1); i++) { // For each sensor
            sensorValues[0][i] = (int)(readADC(hadc, analogReadPins[i])) / norm;//Read each sensor
            HAL_Delay(5); //Wait 5 ms
            isReady = isReady + ((sensorValues[0][i]) / numRings); //Summing up all readings and taking average
        }
        countUp++;
        if (countUp > 500) {
            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4, GPIO_PIN_RESET); //turn off if accidentally turned on
        }
    }
}

void calibrateSensors() {

    // Doing calibration read
    calibration = 0;
    HAL_Delay(500); //Wait half a second
    for (int i = 0; i <= (numRings-1); i++) { // For each sensor
        uint32_t initialValue = (readADC(hadc, analogReadPins[i])) / norm; //Read each sensor
        sensorValues[0][i] = (int)initialValue;
        HAL_Delay(5); //Wait 5 ms
        calibration = calibration + ((sensorValues[0][i]) / numRings); //Taking the average value for the calibration base.

    }
}

void startUpLCD(LCDController myLCD) {
    myLCD.init(); //initiate the LCD
    myLCD.clear(); //get rid of any remaining text on the screen
    myLCD.home(); //set the cursor to home
}


uint32_t readADC(ADC_HandleTypeDef hadc, uint32_t channel) {
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel = channel;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&hadc, &sConfig);

	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);

	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
	uint32_t raw = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);

	sConfig.Rank = ADC_RANK_NONE;
	HAL_ADC_ConfigChannel(&hadc, &sConfig);
	return raw;
}
