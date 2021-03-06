#pragma config(UserModel, "C:/Users/Kevin J/Documents/randomGithub/TEJ4M-RobotC-Programs/PhysicalNamingScheme.h")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//


void stopMoving() {
	motor[motLeft] = 0;
	motor[motRight] = 0;
}

void turn(char direction){
	int modifier;

	if(direction == 'l'){ //turn left
		modifier = -1;
		} else { // turn right
		modifier = 1;
	}

	wait1Msec(1500); // make sure robot is stopped

	motor[motRight] = 64 * modifier;
	motor[motLeft] = -1 * 64 * modifier;

	wait1Msec(1250);
	stopMoving();
}

////////////////
void turn45(char direction) {
	int modifier;

	if(direction == 'l'){ //turn left
		modifier = -1;
		} else { // turn right
		modifier = 1;
	}

	wait1Msec(1500); // make sure robot is stopped

	motor[motRight] = 64 * modifier;
	motor[motLeft] = -1 * 64 * modifier;

	wait1Msec(625);
	stopMoving();
}


void setMotors(int motLeftLvl, int motRightLvl) {
	motor[motLeft] = motLeftLvl;
	motor[motRight] = motRightLvl;
}

task main()
{
	while(true) {
		setMotors(100, 100);
		stopMoving();
		wait1Msec(1000);
		for(int i = 0; i< 4; i++){
			turn('r');
		}

		for(int i = 0; i< 4; i++){
			turn('l');
		}

	}
}
