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
	int modifier;



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

void setMotors(int lvl){
	motor[motLeft] = lvl;
	motor[motRight] = lvl;
}

//Keeps the bot moving forward until
void forwardUntilBigEnough(tMotor m1, tMotor m2, tSensors sensor, int desiredVal){

	while(SensorValue[sensor] < desiredVal){
		setMotors(m1, m2, baseSpeed);
		wait1Msec(waitTime);
	}
	setMotors(m1, m2, 0);


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

void actualJitterMove(int timePerJitter){
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


//turns until the robot is facing its original direction
void turnUntilReset(){
	const int turnTolerance = 10;//1 degree each way

	while(abs(SensorValue[gyro] % 3600) > turnTolerance ){
		motor[motRight] = -1 * turnSpeed;
		motor[motLeft] = turnSpeed;

	}

	stopMoving();
}
//turns to an aboluste degree relative to the robots starting heading
void turnAbsDegree(int degree){
	const int turnTolerance = 7;//0.7 degree each way

	while(abs(SensorValue[gyro] % 3600) < turnTolerance + (degree * 10)
					&& abs(SensorValue[gyro] % 3600) > turnTolerance - (degree * 10)){
		motor[motRight] = -1 * turnSpeed;
		motor[motLeft] = turnSpeed;
	}
}

//////////////////////////////////////
////////////////////////////////////
task main() {

	const int BLACK = 1900;
	startTask(getRidOfArm);
	move(5);//get out of red box
	forwardUntilBigEnough(motLeft, motRight, midLine, BLACK);//get to semicircle
	turnDegree(20);//turn 20 degrees

	while(SensorValue[sonar] > 4){ //go around semicircle until hit wall
		actualJitterMove(150);
	}

	wait1Msec(1000);
	setMotors(motLeft, motRight, -50); //back up
	wait1Msec(1000);
	turnUntilReset();//reset the heading.
	wait1Msec(1000);
	//move to horizontal black line
	forwardUntilBigEnough(motLeft, motRight, midLine, BLACK);
	move(8);
	turn('r');
	//reach middle line 1
	forwardUntilBigEnough(motLeft, motRight, midLine, BLACK);
	move(2);
	forwardUntilBigEnough(motLeft, motRight, midLine, BLACK); //reach middle line 2

	move(8);
	turn('r');
	move(8);

	///////////////////////////////////
	//all over again
///////////////////////////////////////////////
	move(5);//get out of red box
	forwardUntilBigEnough(motLeft, motRight, midLine, BLACK);//get to semicircle
	turnDegree(20);//turn 20 degrees

	while(SensorValue[sonar] > 4){ //go around semicircle until hit wall
		actualJitterMove(150);
	}

	wait1Msec(1000);
	setMotors(motLeft, motRight, -50); //back up
	wait1Msec(1000);
	turnDegree(70);
	wait1Msec(1000);
	//move to bottom red box
	forwardUntilBigEnough(motLeft, motRight, midLine, BLACK);
	move(33); //get into the boxs

}
