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

const int baseSpeed = 70;


void resetEncoders(){
	//Reset encoder values to zero
	SensorValue[encodeRight] = 0;
	SensorValue[encodeLeft] = 0;
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
	float numDegreeRot = (360 / 6.926) * dist;


	resetEncoders();
	//do the actual moving
	while(abs(SensorValue[encodeRight]) < abs(numDegreeRot) ||
		abs(SensorValue[encodeLeft]) < abs(numDegreeRot)){

			if(SensorValue[sonar] > 15 || SensorValue[sonar] == -1){

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
			} else {
				clearLCDLine(0);
				displayNextLCDString("Object Detected!");
				stopMoving();

			}

	}
	stopMoving();
}

void turn(char direction){
	resetEncoders();
	stopMoving();
	int modifier;



	wait1Msec(1200); // make sure robot is stopped
	int numDegreeRot = 362 * rand() % 500 + 1;


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


task getRidOfArm(){
	startMotor(motArm, -127);
	wait1Msec(1250);
	stopMotor(motArm);
}

void setMotors(int lvl){
	motor[motLeft] = lvl;
	motor[motRight] = lvl;
}


task main() {
	const int BLACK = 2000;
	startTask(getRidOfArm);
	while(true){
		while(SensorValue[midLine] < BLACK) {//while not black line, move forwards
			setMotors(baseSpeed);
		}
		move(-6);
		turn('l');
		turn('l');

	}
}
