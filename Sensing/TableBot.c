/**
 * Kevin Jiang
 * March 29 2019
 * The robot moves forward until it hits the edge.
 * It then moves back, turns, and moves forward again.
 * This is continued until the time runs out.
 * Makes use of the motor encoders.
 */


#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    leftLine,       sensorLineFollower)
#pragma config(Sensor, in2,    midLine,        sensorLineFollower)
#pragma config(Sensor, in3,    rightLine,      sensorLineFollower)
#pragma config(Sensor, in4,    gyro,           sensorGyro)
#pragma config(Sensor, in6,    armPot,         sensorPotentiometer)
#pragma config(Sensor, dgtl1,  encodeRight,    sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  encodeLeft,     sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  touchSensor,    sensorTouch)
#pragma config(Sensor, dgtl8,  sonar,          sensorSONAR_cm)
#pragma config(Sensor, I2C_1,  rightIME,       sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  leftIME,        sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  armIME,         sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           motRight,      tmotorVex393_MC29, openLoop, reversed, driveRight, encoderPort, I2C_1)
#pragma config(Motor,  port3,           motLeft,       tmotorVex393_MC29, openLoop, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port6,           motArm,        tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(DatalogSeries, 0, "LeftFollow", Sensors, Sensor, in1, 50)
#pragma config(DatalogSeries, 1, "MidFollow", Sensors, Sensor, in2, 50)
#pragma config(DatalogSeries, 2, "RightFollow", Sensors, Sensor, in3, 50)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

const int baseSpeed = 70; //a base movement speed for the robot to go.
							// is in a constant for easy configuration/customization

/**
 * Resets the left and right encoders to zero
 * Useful for the distance based movement methods.
 */
void resetEncoders(){
	//Reset encoder values to zero
	SensorValue[encodeRight] = 0;
	SensorValue[encodeLeft] = 0;
}

/**
 * Stops the left and right motors.
 * Waits for 250ms to make sure the robot has stopped / has no more momentum.
 */
void stopMoving(){
	startMotor(motLeft, 0);
	startMotor(motRight, 0);
	wait1Msec(250);
}


/**
 * Moves the robot a certain distance.
 * The distance is in inches (since each little box is 3 inches on the scenarios, allows for easy programming)
 * The distance can be positive or negative, depending on direction
 */
void move(int dist){
	int modifier;

	// set the modifier for the motor power based on if the robot is supposed to go forwards or backwards.
	if(dist > 0){
		modifier = 1;
		} else {
		modifier = -1;
	}

	// distance can only be positive. Direction doesn't matter since it is already accounted for.
	dist = abs(dist);


	// convert distance in inches to the number of degrees the wheels must rotate.
	// based on the fact that the wheels have a circumference of 2.8cm
	float numDegreeRot = (360 / 6.926) * dist;

	// reset encoders to have a clean slate
	resetEncoders();

	// keep moving if any of the wheels haven't reached their rotation quota
	while(abs(SensorValue[encodeRight]) < abs(numDegreeRot) ||
		abs(SensorValue[encodeLeft]) < abs(numDegreeRot)){

		//left wheels
		if(abs(SensorValue[encodeRight]) < abs(numDegreeRot)) {
			startMotor(motLeft, 127 * modifier); //if the left wheels haven't reached their rotation quota, keep them rotating
			} else {
			stopMotor(motLeft); //if they have, stop the left wheels
		}

		//right wheels
		if(abs(SensorValue[encodeLeft]) < abs(numDegreeRot)) {
			startMotor(motRight, 127 * modifier); //if the right wheels haven't reached their rotation quota, keep them rotating
			} else {
			stopMotor(motRight); //if they have, stop the right wheels
		}
	}

	// stop the robot to prevent it from messing with proceeding methods.
	stopMoving();
}

/**
 * Turn in a direction.
 * Since the point of this challenge is just to move around, the direction given as a parameter is a suggestion.
 */
void turn(char direction){
	int modifier;
	int numDegreeRot = 362 * rand() % 500 + 1; //determine the number of degrees to rotate based on a random number

	stopMoving(); //make sure robot isn't moving anymore
	resetEncoders(); //make sure encoder values are clean for the turn

	wait1Msec(1200); // make sure robot is stopped

	if(direction == 'l'){ //turn "left"
		modifier = -1;
		} else { // turn "right"
		modifier = 1;
	}
	resetEncoders();

	//while any one of them haven't reached their turning quotas...
	while(abs(SensorValue[encodeRight]) < abs(numDegreeRot) ||
		abs(SensorValue[encodeLeft]) < abs(numDegreeRot)){


				//if it's the left that didn't reach the quota, let the left wheel turn
				if(abs(SensorValue[encodeLeft]) < abs(numDegreeRot)) {
					startMotor(motLeft, 63 * modifier);
					} else {
					stopMotor(motLeft);
				}
				//if it's the right that didn't reach the quota, let the right wheel turn
				if(abs(SensorValue[encodeRight]) < abs(numDegreeRot)) {
					startMotor(motRight, 63 * -1 * modifier);
					} else {
					motor[motRight] = 0;

				}
		//wait to make sure the CPU isn't always overloaded
		wait1Msec(50);

	}

	//stop the motors to keep this method self contained.
	stopMoving();
}

/**
 * Gets rid of the arm.
 * Useful in case the arm gets in the way of movements, hits obstacles, etc.
 */
task getRidOfArm(){
	startMotor(motArm, -127);
	wait1Msec(1250);
	stopMotor(motArm);
}

/**
 * Sets the left and right motors to an integer "lvl".
 * lvl is between -127 and 127.
 */
void setMotors(int lvl){
	motor[motLeft] = lvl;
	motor[motRight] = lvl;
}


task main() {

	const int BLACK = 2000; //if the line follower has a reading of >2000, then it is over a black line
	startTask(getRidOfArm); //get rid of the arm since it isn't necessary


	while(true){
		//while not black line, move forwards
		while(SensorValue[midLine] < BLACK) {
			setMotors(baseSpeed);
		}

		//once it turns black...
		move(-6); //move back 6 inches
		turn('l'); // turn some amount of degrees
		turn('l'); // turn some amount of degrees again.

	}
}
