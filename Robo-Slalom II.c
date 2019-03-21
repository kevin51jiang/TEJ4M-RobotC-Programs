#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    leftLine,       sensorLineFollower)
#pragma config(Sensor, in2,    midLine,        sensorLineFollower)
#pragma config(Sensor, in3,    rightLine,      sensorLineFollower)
#pragma config(Sensor, in4,    gyro,           sensorGyro)
#pragma config(Sensor, in6,    armPot,         sensorPotentiometer)
#pragma config(Sensor, dgtl1,  rightEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  leftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  touchSensor,    sensorTouch)
#pragma config(Sensor, dgtl8,  sonarSensor,    sensorSONAR_cm)
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

const float wheelRadius = 2.8;

void whack(){
	startMotor(motArm, -127);
	wait1Msec(1500);
	startMotor(motArm, 127);
	wait1Msec(4000);
	startMotor(motArm, -127);
	wait1Msec(1500);
}

void resetEncoders(){
	//Reset encoder values to zero
	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;
}

void stopMoving(){

	startMotor(motLeft, 0);
	startMotor(motRight, 0);
	wait1Msec(250);
}


//distance in inches
void move(int dist){
	int modifier;

	if(dist > 0){
		modifier = 1;
		} else {
		modifier = -1;
		dist *= -1;
	}

	/*
	//convert distance into number of rotations
	float numRot = dist / 6.926;
	//convert number of rotations into degrees
	int numDegreeRot = 360 * numRot;
	*/
	float numDegreeRot = (360 /  (wheelRadius * 2 * 3.14159) / 2.54) * dist;


	resetEncoders();
	//do the actual moving
	while(abs(SensorValue[rightEncoder]) < abs(numDegreeRot) ||
		abs(SensorValue[leftEncoder]) < abs(numDegreeRot)){



		//left wheels
		if(abs(SensorValue[rightEncoder]) < abs(numDegreeRot)) {
			startMotor(motLeft, 127 * modifier);
			} else {
			stopMotor(motLeft);
		}
		//right wheels
		if(abs(SensorValue[leftEncoder]) < abs(numDegreeRot)) {
			startMotor(motRight, 127 * modifier);
			} else {
			stopMotor(motRight);
		}

	}
	stopMoving();
}

void turn(char direction){
	resetEncoders();
	stopMoving();
	int modifier;



	wait1Msec(1200); // make sure robot is stopped
	const int numDegreeRot = 359;


	if(direction == 'l'){ //turn left
		modifier = -1;
		} else { // turn right
		modifier = 1;
	}
	resetEncoders();
	//do the actual moving
	while(abs(SensorValue[rightEncoder]) < abs(numDegreeRot) ||
		abs(SensorValue[leftEncoder]) < abs(numDegreeRot)){
		//left wheels
		if(abs(SensorValue[leftEncoder]) < abs(numDegreeRot)) {
			startMotor(motLeft, 63 * modifier);
			} else {
			stopMotor(motLeft);
		}
		//right wheels
		if(abs(SensorValue[rightEncoder]) < abs(numDegreeRot)) {
			startMotor(motRight, 63 * -1 * modifier);
			} else {
			motor[motRight] = 0;
		}

		wait1Msec(50);

	}
	stopMoving();
}


task getRidOfArm(){
	startMotor(motArm, -127);
	wait1Msec(1250);
	stopMotor(motArm);
}


int jitter(tMotor mPrimary, tMotor mSecondary, int jitterTime){
	const int speed = 63;
	startMotor(mPrimary, speed);
	startMotor(mSecondary, -1 * speed);
	wait1Msec(jitterTime / 2);
	stopMotor(mPrimary);
	stopMoving();
	int sensorReading = SensorValue[midLine];
	startMotor(mPrimary, -1 * speed);
	startMotor(mSecondary, speed);
	wait1Msec(jitterTime / 2);
	stopMoving();
	datalogAddValueWithTimeStamp(0, sensorReading);
	return sensorReading;
}

/**
* jitterTime = time in ms for one half of a jitter cycle to happen (e.g. move from center to right and back to center)
* duration = how long the whole jitter move should be in ms
*/
void jitterMove(int jitterTime, int duration){
	int numJitter = duration / jitterTime;

	for(int i = 0; i < numJitter / 2; i++) {
		//jitter right
		jitter(motLeft, motRight, jitterTime);
		//jitter left
		jitter(motRight, motLeft, jitterTime);
	}
}



task main()
{
	const int timePerJitter = 100;
	startTask(getRidOfArm);
	bool isLeft = true;
	while(true){
		//Plan
		//Jitter both ways, figure out which returns the more +ve, number, then only move in that direction(using only 1 motor)
		int leftReading, rightReading;
		leftReading = jitter(motRight, motLeft, timePerJitter);

		rightReading = jitter(motLeft, motRight, timePerJitter);
		tMotor toMove;
		if(leftReading > rightReading){
			toMove = motRight;
		} else {
			toMove = motLeft;
		}
		startMotor(toMove, 100);
		wait1Msec(timePerJitter * 3/4);
		stopMotor(toMove);
		wait1Msec(50);


	}
}
