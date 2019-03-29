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

const int baseSpeed = 120; // the base speed for moving
const int turnSpeed = 70; // the default speed when turning
const int waitTime = 50; // amount of wait time before checking sensors again. Prevents always using 100% CPU.

/**
 * Sets the left and right encoder values to 0.
 * Useful for movement functions.
 */
void resetEncoders(){
	//Reset encoder values to zero
	SensorValue[encodeRight] = 0;
	SensorValue[encodeLeft] = 0;
}

/**
 * Sets the left and right motor speeds to zero, stopping the robot.
 */
void stopMoving(){
	startMotor(motLeft, 0);
	startMotor(motRight, 0);
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
 * Sets m0 and m1 to the power level of integer lvl
 */
void setMotors(tMotor m0, tMotor m1, int lvl){
	motor[m0] = lvl;
	motor[m1] = lvl;
}

/**
 * Keeps moving forward on m1, m2 until sensor is <= desiredVal
 */
void forwardUntilSmallEnough(tMotor m1, tMotor m2, tSensors sensor, int desiredVal){

	while(SensorValue[sensor] > desiredVal && SensorValue[sensor] != -1){// while desiredVal is less than the sensor value, and the sensor has a valid output
		//keep the motors turning
		setMotors(m1, m2, baseSpeed);
		wait1Msec(waitTime);
	}

	//stop the motors to keep this method self contained.
	setMotors(m1, m2, 0);
}


/**
 * Using the gryo sensor, the robot turns until it is facing in the direction that it started in at the very beginning
 */
void turnUntilReset(){
	const int turnTolerance = 10;//10 tenths of a degree's tolerance in each direction

	while(abs(SensorValue[gyro] % 3600) > turnTolerance ){ //while it is not within the tolerance level relative to zero degrees ...
		// continuously clockwise
		motor[motRight] = -1 * turnSpeed;
		motor[motLeft] = turnSpeed;
	}

	//stop the motors to keep this method self contained.
	stopMoving();
}

/**
 * Sets the direction of the robot to be a certain amount of degrees clockwise of its starting position.
 */
void setHeading(int degreesHeading){
	const int turnTolerance = 1; //tolerance level of 1 degree in each direction

	// while the difference between the gyro reading in degrees and the destination degrees is greater than the tolerance
	while(abs(SensorValue[gyro] / 10 - degreesHeading) > turnTolerance ){
		//keep turning clockwise
		motor[motRight] = -1 * turnSpeed;
		motor[motLeft] = turnSpeed;
		wait1Msec(100);
	}

	//stop the motors to keep this method self contained.
	setMotors(motLeft, motRight, 0);
}

/**
 * Sets the motor, m1, to the desiredEncoderPos which is read from the sensor pot
 * The variables are named this way because it is mostly used with the robot arms / potentiometer
 */
void setMotorToPos(tMotor m1, tSensors pot, int desiredEncoderPos){
	int direction;

	// set the correct direction for the motor movement
	if(SensorValue[pot] == desiredEncoderPos) {
		return; //nothing to do if they're already equal

	} else if(SensorValue[pot] < desiredEncoderPos) { // means want to move it upwards (relative to the horizontal starting position)
		direction = 1;

	} else {
		direction = -1; // the arm should move downwards (relative to the horizontal starting position)

	}

	//get it so that it is within 10 steps of desired outcome
	while(abs(SensorValue[pot] - desiredEncoderPos) > 10 / 2) {
		// use turnspeed since it's slower (should be more accurate)
		// especially important because the arm motor can't just wrap around like the movement motors can
		motor[m1] = direction * turnSpeed;
	}

	motor[m1] = 0; //stop the motor once it's at the right position to keep this method self contained.
}


//////////////////////////////////////
////////////////////////////////////
task main() {
	// constants of what the potentiometer should read if the arm is at a certain position
	const int DIAGONAL = 2100,
						ALLDOWN = 4095;

	// move forward until the robot is within arms length of the ball
	forwardUntilSmallEnough(motLeft, motRight, sonar, 9);

	setMotorToPos(motArm, armPot, DIAGONAL); //pickup the ball
	wait1Msec(1000);
	move(-5); //move back 5 inches
	wait1Msec(1000);
	setHeading(90); //turn right
	wait1Msec(1000);
	move(12); // move forward 1 foot
	wait1Msec(1000);
	turnUntilReset(); // turn left
	wait1Msec(1000);
	move(4 * 12); // move forward 4 feet
	wait1Msec(1000);
	setHeading(270); // turn left
	wait1Msec(1000);
	move(12 + 1); //move forward 13 inches
	wait1Msec(1000);
	turnUntilReset(); // turn right
	wait1Msec(1000);
	move(1); // move 1 inch forward
	wait1Msec(1000);
	setMotorToPos(motArm, armPot, ALLDOWN); // drop the arm to allow the ball to roll off into the hoop

}
