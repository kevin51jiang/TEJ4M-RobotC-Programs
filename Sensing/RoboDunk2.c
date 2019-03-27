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

const int baseSpeed = 120;
const int turnSpeed = 70;
const int waitTime = 50;

void resetEncoders(){
	//Reset encoder values to zero
	SensorValue[encodeRight] = 0;
	SensorValue[encodeLeft] = 0;
}

void stopMoving(){

	startMotor(motLeft, 0);
	startMotor(motRight, 0);
}


//distance in inches
void move(int dist){
	int modifier;

	if(dist > 0){
		modifier = 1;
		} else {
		modifier = -1;
		dist = abs(dist);
	}
	/*
	//convert distance into number of rotations
	float numRot = dist / 6.926;
	//convert number of rotations into degrees
	int numDegreeRot = 360 * numRot;
	*/
	float numDegreeRot = (360 / 6.926) * dist;


	resetEncoders();
	//do the actual moving
	while(abs(SensorValue[encodeRight]) < abs(numDegreeRot) ||
		abs(SensorValue[encodeLeft]) < abs(numDegreeRot)){


		//left wheels
		if(abs(SensorValue[encodeRight]) < abs(numDegreeRot)) {
			startMotor(motLeft, 127 * modifier);
			} else {
			stopMotor(motLeft);
		}
		//right wheels
		if(abs(SensorValue[encodeLeft]) < abs(numDegreeRot)) {
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
	int numDegreeRot = 361;


	if(direction == 'l'){ //turn left
		modifier = -1;
		} else { // turn right
		modifier = 1;
	}
	resetEncoders();
	//do the actual moving
	while(abs(SensorValue[encodeRight]) < abs(numDegreeRot) ||
		abs(SensorValue[encodeLeft]) < abs(numDegreeRot)){


		//left wheels
		if(abs(SensorValue[encodeLeft]) < abs(numDegreeRot)) {
			startMotor(motLeft, 63 * modifier);
			} else {
			stopMotor(motLeft);
		}
		//right wheels
		if(abs(SensorValue[encodeRight]) < abs(numDegreeRot)) {
			startMotor(motRight, 63 * -1 * modifier);
			} else {
			motor[motRight] = 0;

		}
		wait1Msec(50);

	}
	stopMoving();
}



void turnDegree(int degrees){
	resetEncoders();
	stopMoving();

	wait1Msec(1200); // make sure robot is stopped
	int numDegreeRot = degrees * 4;

	resetEncoders();
	//do the actual moving
	while(abs(SensorValue[encodeRight]) < abs(numDegreeRot) ||
		abs(SensorValue[encodeLeft]) < abs(numDegreeRot)){


		//left wheels
		if(abs(SensorValue[encodeLeft]) < abs(numDegreeRot)) {
			startMotor(motLeft, 63);
			} else {
			stopMotor(motLeft);
		}
		//right wheels
		if(abs(SensorValue[encodeRight]) < abs(numDegreeRot)) {
			startMotor(motRight, 63 * -1 );
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

void setMotors(tMotor m0, tMotor m1, int lvl){
	motor[m0] = lvl;
	motor[m1] = lvl;
}

//void setMotors(int lvl){
//	motor[motLeft] = lvl;
//	motor[motRight] = lvl;
//}

//Keeps the bot moving forward until
void forwardUntilBigEnough(tMotor m1, tMotor m2, tSensors sensor, int desiredVal){

	while(SensorValue[sensor] < desiredVal){
		setMotors(m1, m2, baseSpeed);
		wait1Msec(waitTime);
	}
	setMotors(m1, m2, 0);


}

void forwardUntilSmallEnough(tMotor m1, tMotor m2, tSensors sensor, int desiredVal){

	while(SensorValue[sensor] > desiredVal && SensorValue[sensor] != -1){
		setMotors(m1, m2, baseSpeed);
		wait1Msec(waitTime);
	}
	setMotors(m1, m2, 0);


}



//turns until the robot is facing its original direction
void turnUntilReset(){
	const int turnTolerance = 10;//1 degree each way

	while(abs(SensorValue[gyro] % 3600) > turnTolerance ){
		motor[motRight] = -1 * turnSpeed;
		motor[motLeft] = turnSpeed;

	}

	stopMoving();
}


//turns until the robot is facing its original direction
//DO NOT Use when degreesHeading == 0 (actually I should squish the 2 funcitons together someitme)
void setHeading(int degreesHeading){
	const int turnTolerance = 10;//1 degree each way

	while(abs(SensorValue[gyro] / 10 - degreesHeading) > turnTolerance ){
		motor[motRight] = -1 * turnSpeed;
		motor[motLeft] = turnSpeed;
		wait1Msec(100);

	}
	setMotors(motLeft, motRight, 0);

	stopMoving();
}

void setMotorToPos(tMotor m1, tSensors pot, int desiredEncoderPos){
	//make sure that it's moving the right way
	int direction = -1; // by default, move downwards
	if(SensorValue[pot] < desiredEncoderPos) { // means want to move it upwards (relative to starting position)
		direction = 1;
	}

	//get it so that it is within 10 steps of desired outcome
	while(abs(SensorValue[pot] - desiredEncoderPos) > 10 / 2) {
		motor[m1] = direction * turnSpeed;	//use turnspeed since it's slower (should be more accurate)
	}
	motor[m1] = 0; //stop the motor once it's at the right position

}


//////////////////////////////////////
////////////////////////////////////
task main() {

	const int HORIZONTAL = 3200,
						DIAGONAL = 2100,
						ALLDOWN = 4095;


	forwardUntilSmallEnough(motLeft, motRight, sonar, 9);

	setMotorToPos(motArm, armPot, DIAGONAL);
	wait1Msec(1000);
	move(-5);
	wait1Msec(1000);
	setHeading(90);
	wait1Msec(1000);
	move(12);
	wait1Msec(1000);
	turnUntilReset();
	wait1Msec(1000);
	move(4 * 12);
	wait1Msec(1000);
	setHeading(270);
	wait1Msec(1000);
	move(12 + 1);
	wait1Msec(1000);
	turnUntilReset();
	wait1Msec(1000);
	move(2);
	wait1Msec(1000);
	setMotorToPos(motArm, armPot, ALLDOWN);

}
