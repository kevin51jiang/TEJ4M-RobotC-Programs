#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    leftLine,       sensorLineFollower)
#pragma config(Sensor, in2,    midLine,        sensorLineFollower)
#pragma config(Sensor, in3,    rightLine,      sensorLineFollower)
#pragma config(Sensor, in4,    gyro,           sensorGyro)
#pragma config(Sensor, in6,    amotLeftPot,         sensorPotentiometer)
#pragma config(Sensor, dgtl1,  encodeRight,    sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  encodeLeft,     sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  touchSense,     sensorTouch)
#pragma config(Sensor, dgtl8,  sonar,          sensorSONAR_cm)
#pragma config(Sensor, I2C_1,  rightIME,       sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  leftIME,        sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  amotLeftIME,         sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           motLeft,      tmotorVex393_MC29, openLoop, reversed, driveRight, encoderPort, I2C_1)
#pragma config(Motor,  port3,           motRight,       tmotorVex393_MC29, openLoop, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port6,           motArm,        tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(DatalogSeries, 0, "LeftFollow", Sensors, Sensor, in1, 50)
#pragma config(DatalogSeries, 1, "MidFollow", Sensors, Sensor, in2, 50)
#pragma config(DatalogSeries, 2, "RightFollow", Sensors, Sensor, in3, 50)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/**
 * Kevin Jiang
 * March 29 2019
 * Robotics Academy Challenge
 * Completes the robotics academy challenge.
 * Picks up the ball, drops it in the hoop, follows the line, and makes it to the finish line.
 * Makes use of gyro, motor encoder/potentiometers, and sonar sensors.
 */




const int baseSpeed = 120; // base movement speed of robot
const int turnSpeed = 35; // base turn speed of robot
const int waitTime = 25; //waiting between sensor checks
const int restTime = 1000;//rest time in between commands

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
 * Sets the power levels of the left and right motors to 0.
 * Stops the robot from moving.
 */
void stopMoving(){
	startMotor(motRight, 0);
	startMotor(motLeft, 0);
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
			startMotor(motRight, 127 * modifier); //if the left wheels haven't reached their rotation quota, keep them rotating
			} else {
			stopMotor(motRight); //if they have, stop the left wheels
		}

		//right wheels
		if(abs(SensorValue[encodeLeft]) < abs(numDegreeRot)) {
			startMotor(motLeft, 127 * modifier); //if the right wheels haven't reached their rotation quota, keep them rotating
			} else {
			stopMotor(motLeft); //if they have, stop the right wheels
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
 * Keeps the robot moving forward using motors m1 and m2, until the sensor reading is greater than desiredVal
 */
void forwardUntilBigEnough(tMotor m1, tMotor m2, tSensors sensor, int desiredVal){
	//while the value is less than the desiredVal
	while(SensorValue[sensor] < desiredVal){
		// continue moving
		setMotors(m1, m2, baseSpeed);
		wait1Msec(waitTime);
	}

	// keep the method self contained
	setMotors(m1, m2, 0);
}

/**
 * Keeps the robot moving forward using motors m1 and m2, until the sensor reading is less than desiredVal
 */
void forwardUntilSmallEnough(tMotor m1, tMotor m2, tSensors sensor, int desiredVal){

	//while the sensor gives invalid input (-1), or it is greater than the desiredVal
	while(SensorValue[sensor] > desiredVal || SensorValue[sensor] == -1){
		// continue moving
		setMotors(m1, m2, baseSpeed);
		wait1Msec(waitTime);
	}

	//stop the motors to keep the method self contained.
	setMotors(m1, m2, 0);


}

/**
 * Using the gryo sensor, the robot turns until it is facing in the direction that it started in at the very beginning
 */
void turnUntilReset(){
	const int turnTolerance = 10;//10 tenths of a degree's tolerance in each direction

	while(abs(SensorValue[gyro] % 3600) > turnTolerance ){ //while it is not within the tolerance level relative to zero degrees ...
		// continuously clockwise
		motor[motLeft] = -1 * turnSpeed;
		motor[motRight] = turnSpeed;
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
		motor[motLeft] = -1 * turnSpeed;
		motor[motRight] = turnSpeed;
		wait1Msec(100);
	}

	//stop the motors to keep this method self contained.
	setMotors(motRight, motLeft, 0);
}



/**
 * Sets the motor, m1, to the desiredEncoderPos which is read from the sensor pot
 * The variables are named this way because it is mostly used with the robot amotLefts / potentiometer
 */
void setMotorToPos(tMotor m1, tSensors pot, int desiredEncoderPos){
	int direction;

	// set the correct direction for the motor movement
	if(SensorValue[pot] == desiredEncoderPos) {
		
		return; //nothing to do if they're already equal
		
	} else if(SensorValue[pot] < desiredEncoderPos) { // means want to move it upwards (relative to the horizontal starting position)
		
		direction = 1;

	} else {
		
		direction = -1; // the amotLeft should move downwards (relative to the horizontal starting position)

	}

	//get it so that it is within 50 steps of desired outcome
	while(abs(SensorValue[pot] - desiredEncoderPos) > 50 / 2) {
		// use turnspeed since it's slower (should be more accurate)
		// especially important because the amotLeft motor can't just wrap around like the movement motors can
		motor[m1] = direction * turnSpeed;	
	}

	motor[m1] = 0; //stop the motor once it's at the right position to keep this method self contained.
}



/**
 * Using the movement motors m1 and m2, 
 * the robot will continue following a line until what the sensor's value is lower than desiredVal
 */
void lineFollowUntilSmallEnough(tMotor m1, tMotor m2, tSensors sensor, int desiredVal){
	// while the sensor returns a valid value and it is greater than desiredVal
	while(SensorValue[sensor] != -1 && SensorValue[sensor] > desiredVal ){
		
		// follow the line
		if(SensorValue[leftLine] > SensorValue[rightLine]){ //if the left is darker than the right sensor
			// turn left
			motor[m1] = 20;
			motor[m2] = 63; 
		} else { // if the right is darker than the left sensor
		 	// turn right
			motor[m1] = 63;
			motor[m2] = 20;
		}

	}

	// stop the motors to keep the method self contained
	setMotors(m1, m2, 0);
}

/**
 * Using the movement motors m1 and m2, 
 * the robot will continue following a line until what the sensor reads is within an acceptable tolerance of desiredVal
 */
void lineFollowUntilEqual(tMotor m1, tMotor m2, tSensors sensor, int desiredVal, int tolerance){
	//while the difference between the sensor value and the desired value is greater than the tolerance
	while(abs(SensorValue[sensor] - desiredVal) > tolerance){ 

		// follow the line
		if(SensorValue[leftLine] > SensorValue[rightLine]){ //if the left is darker than the right sensor
			// turn left
			motor[m1] = 20;
			motor[m2] = 63; 
		} else { // if the right is darker than the left sensor
		 	// turn right
			motor[m1] = 63;
			motor[m2] = 20;
		}
	}
	
	// stop the motors to keep the method self contained
	setMotors(m1, m2, 0);
}



//////////////////////////////////////
////////////////////////////////////
task main() {

	// a bunch of constants related to sensors
	const int HORIZONTAL = 3240, // Pot: moves the amotLeft into a horizontal position
						DIAGONAL = 2100,// Pot: moves amotLeft ~45 degrees above horizontal

						BLACK = 2000, // Line sensor: anything higher than 2000 is black
						PICKUPRANGE = 9;// Sonar: optimal range for picking objects up with amotLeft. Cm.

	///////////////////////////////
	// ------ INSTRUCTIONS ----- //
	///////////////////////////////
	//Start from A
	//
	//Grab the first blue ball
	//
	//Deposit in the center hoop
	//
	//Turn, and continue forward (turnuntilreset)
	//
	//Forward, forward etc. until reach semicircle
	//
	//Follow the semicirlcle Until point A
	//
	//Go forward until the "start" aka turn left
	//Follow the resulting arrows until you reach the end box
	//Stop. END.


	move(4);//move cross the starting line
	forwardUntilBigEnough(motRight, motLeft, midLine, BLACK);//move forward to the first line with the ball
	move(2);//center the robot
	wait1Msec(restTime);

	//pickup the ball
	setHeading(90);//turn right
	wait1Msec(restTime);
	forwardUntilSmallEnough(motRight, motLeft, sonar, PICKUPRANGE); //move close enough to pole to pick up
	setMotorToPos(motArm, amotLeftPot, DIAGONAL);//pickup the ball
	move(-6);//move 6 inches back
	turnUntilReset();//reset heading

	//move across 3 marker lines
	for(int i = 0; i < 3; i++){
		move(4);//move across that black line
		forwardUntilBigEnough(motRight, motLeft, midLine, BLACK);
	}


	//deposit ball in hoop
	wait1Msec(restTime);
	setHeading(90); // turn right
	wait1Msec(restTime);
	forwardUntilSmallEnough(motRight, motLeft, sonar, PICKUPRANGE); //move close enough to pole to pick up
	setMotorToPos(motArm, amotLeftPot, HORIZONTAL);//drop the ball
	wait1Msec(2500);//wait for ball to drop
	motor[motArm] = -127;
	wait1Msec(2500);//wait for amotLeft to retract
	motor[motArm] = 0;
	move(-6);//move 6 inches back
	turnUntilReset();//reset heading
	move(5); //move robot past black line

	//move towards the semicircle
	for(int i = 0; i < 4; i++){
		move(3);
		forwardUntilBigEnough(motRight, motLeft, midLine, BLACK);
	}

	//set it oriented in roughly the right direction, and move forward until it hits black again
	setHeading(50);
	forwardUntilBigEnough(motRight, motLeft, midLine, BLACK);

	lineFollowUntilSmallEnough(motRight, motLeft, sonar, 4);//follow line until robot AmotRightOST hits wall
	move(-5);
	turnUntilReset();//reset heading
	move(5);//move across the 1st line

	//move across 5 lines to reach the gap in between the walls
	for(int i = 0; i < 5; i++){
		move(3);
		forwardUntilBigEnough(motRight, motLeft, midLine, BLACK);
	}

	//turn left
	setHeading(270);
	wait1Msec(restTime);

	//move towards the first curved arrow
	move(12 * 3);
	forwardUntilBigEnough(motRight, motLeft, midLine, BLACK);

	//turn and move forward until it hits the second curved arrow
	turnUntilReset();
	forwardUntilBigEnough(motRight, motLeft, midLine, BLACK);//hit last arrow

	//once it hits, change the heading so that it's slanted towards the line instead of perpendicular
	setHeading(300);

	//follow the line until it faces the opposite of when it started
	lineFollowUntilEqual(motRight, motLeft, gyro, 1800, 5);//until the direction is reversed

	//move forward until it is 10cm away from the wall
	forwardUntilSmallEnough(motRight, motLeft, sonar, 10);//move forward until amotRightost hit the wall in the scorebox

}
