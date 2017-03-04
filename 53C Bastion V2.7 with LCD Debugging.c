#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    leftClawPot,    sensorPotentiometer)
#pragma config(Sensor, in2,    rightClawPot,   sensorPotentiometer)
#pragma config(Sensor, dgtl1,  sensor_Lift,    sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  rightFront,     sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  leftFront,      sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  leftBack,       sensorQuadEncoder)
#pragma config(Sensor, dgtl11, rightBack,      sensorQuadEncoder)
#pragma config(Sensor, I2C_1,  rightIME,       sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  leftIME,        sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           clawRight,     tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           leftDT,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           rightDT,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           topRight,      tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port5,           midRight,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           botRight,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           botLeft,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           midLeft,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           topLeft,       tmotorVex393_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port10,          clawLeft,      tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!
#define e 2.718281828

//Y DRIVETRAIN PROGRAM

//---Motor Connections---//
// Drivetrain -- 2,3 -- DT y'd off
// Lift -- 4,5,6,7,8,9
//Claw -- 1,2

//-------------Initialize Variables-------------//


// Autonomous Mode Variables
int auton = 0;
int left = 1;
int right = 2;

//---Variables for User Control---//
bool reverser = false;
int driverControlModeCount = 1;
bool autoClaw = false;

//---Joystick Variables & Deadzone--//
int Y2 = 0, Y1 = 0, threshold = 15;

//---Constant lift parameters---//
int leftIntegratedEncoderMaxValue = 900;
int leftIntegratedEncoderMinValue = 0;
int liftMaxSpeed = 127;
int up = 1;
int down = -1;

//---Used as to control automatic lift---//
bool autoRaiseLift = false;
bool autoLowerLift = false;
bool liftAutoMode = false;
int lift;

#pragma DebuggerWindows("Globals")
#pragma DebuggerWindows("Motors")

//---User Functions---//


//reading: ticks -- 627.2 ticks/revolution for a torque motor
//reading: ticks -- 392 ticks/revolution for a high speed motor
//reading: ticks -- 261.333 ticks/revolution for a turbo speed motor
//for every 1 turn of the wheels -- 4 inch wheels = 627.2 ticks
//for every 1 turn of each wheel 4*pi inches
//for every 4*pi inches = 627.2*2 ticks
//ticks/inch = 627.2*2/(4*pi) = 99.82198 for torque motor
//ticks/inch = 392*2/(4*pi) = 32.38874 for high speed motor
//ticks/inch = 261.333*2/(4*pi) = 41.59244 for turbo speed motor

//when my error is 30, the actual error in inches = .3

void clearEncoders(){
	SensorValue[leftBack] = 0;
	SensorValue[leftFront] = 0;
	SensorValue[rightBack] = 0;
	SensorValue[rightFront] = 0;
}

//------User Conversions-------//
int inchToTicks (float inch) //torque motor
{
	int ticks;
	ticks = inch*(90/( 4*PI));
	return ticks;
}

int degreesToTicks (float degree) //method is more accurate
{
	int ticksPerTurn = inchToTicks((11*PI)); //this value is just a placeholder, not sure how many ticks per turn, depends on robot, we will have to calculate this
	int ticks = degree * ticksPerTurn / 360;
	return ticks;
}

int timerValue (float seconds)
{
	int miliseconds;
	miliseconds = seconds * 1000;
	if (miliseconds > 250)
	{
		miliseconds = 250;
	}
	return miliseconds;
}

//-------------Robot Functions-------------//

void driveForwardInches(float inch){
	int distance = inchToTicks(inch) * 12.4;
	clearEncoders();
	motor[leftDT] = 50;
	motor[rightDT] = 50;
	while ((SensorValue[leftBack] + SensorValue[leftFront] + SensorValue[rightFront] + SensorValue[rightBack]) < distance){}
	motor[leftDT] = 0;
	motor[rightDT] = 0;
}

void driveBackwardInches(float inch){
	int distance = inchToTicks(inch) * -12.4;
	clearEncoders();
	motor[leftDT] = -50;
	motor[rightDT] = -50;
	while ((SensorValue[leftBack] + SensorValue[leftFront] + SensorValue[rightFront] + SensorValue[rightBack]) > distance){}
	motor[leftDT] = 0;
	motor[rightDT] = 0;
}

void degreeTurn(float degree, int direction){
	int turnTicks = 6.9 * degreesToTicks(degree);
	clearEncoders();
	if (direction == left){
		motor[leftDT] = -50;
		motor[rightDT] = 50;
		while (((SensorValue[leftBack] + SensorValue[leftFront]) >= -turnTicks) && ((SensorValue[rightBack] + SensorValue[rightFront]) <= turnTicks)){}
		motor[leftDT] = 0;
		motor[rightDT] = 0;
	}
	if (direction == right){
		motor[leftDT] = 50;
		motor[rightDT] = -50;
		while (((SensorValue[leftBack] + SensorValue[leftFront]) <= turnTicks) && ((SensorValue[rightBack] + SensorValue[rightFront]) >= -turnTicks)){}
		motor[leftDT] = 0;
		motor[rightDT] = 0;
	}
}

// Stops the drivetrain movement
void stopMovement(){
	motor[leftDT] = 0;
	motor[rightDT] = 0;
}

// Moves the lift. Takes two parameters: direction and speed.
void moveLift(int direction, int liftSpeed){
	lift = direction * liftSpeed;
	motor[topLeft] = lift;
	motor[midLeft] = lift;
	motor[botLeft] = lift;
	motor[topRight] = lift;
	motor[midRight] = lift;
	motor[botRight] = lift;
}

// Calls the holdLift() function to lock the lift
void stopLift(){
	moveLift(up, 0);
	autoLowerLift = false;
	autoRaiseLift = false;
}

void closeClawTogether()
{
	motor[clawRight] = -100;
	motor[clawLeft] = -100;
	while (SensorValue[leftClawPot] < 3450 || SensorValue[rightClawPot] > 600)
	{
		if (SensorValue[leftClawPot] >= 3450){
			motor[clawLeft] = 0;
		}

		if (SensorValue[rightClawPot] <= 600){
			motor[clawRight] = 0;
		}
		if (vexRT[Btn8U] == 1){
			break;
		}

	}

	autoClaw = false;
}

void closeClawCube()
{
	motor[clawRight] = -100;
	motor[clawLeft] = -100;
	while (SensorValue[leftClawPot] < 3200 || SensorValue[rightClawPot] > 760)
	{
		if (SensorValue[leftClawPot] >= 3200){
			motor[clawLeft] = 0;
		}

		if (SensorValue[rightClawPot] <= 760){
			motor[clawRight] = 0;
		}
		if (vexRT[Btn8U] == 1){
			break;
		}

	}

	autoClaw = false;
}

void openClawTogether()
{
	while (SensorValue[leftClawPot] > 2440 && SensorValue[rightClawPot] < 1321)
	{
		motor[clawRight] = 127;
		motor[clawLeft] = 127;
		if (vexRT[Btn8U] == 1){
			break;
		}
	}
	motor[clawRight] = 0;
	motor[clawLeft] = 0;
	autoClaw = false;
}

// Raises the lift
void raiseLift() {
	moveLift(up, liftMaxSpeed);
	/*if (SensorValue[leftIME] == 700)
	{
		openClawTogether();
	}*/
}

// Lower the lift
void lowerLift(){
	moveLift(down, liftMaxSpeed);
}

// Controls for the claw
void openClaw(){
	motor[clawRight] = 127;
	motor[clawLeft] = 127;
}

void closeClaw(){
	motor[clawRight] = -127;
	motor[clawLeft] = -127;
}

void stopClaw(){
	motor[clawRight] = 0;
	motor[clawLeft] = 0;
}

//Slows down the other side if one is going faster... usually the left side
void closeClaw2()
{
	SensorValue[leftClawPot] = 0;
	SensorValue[rightClawPot] = 0;
	if(abs(SensorValue[leftClawPot] - SensorValue[rightClawPot]) < 20)
	{
		motor[clawLeft] = -127;
		motor[clawRight] = -127;
	}
	else if (SensorValue[leftClawPot] > SensorValue[rightClawPot])
	{
		motor[clawLeft] = -127;
		motor[clawRight] = -127;
	}
	else if (SensorValue[leftClawPot] < SensorValue[rightClawPot])
	{
		motor[clawLeft] = -127;
		motor[clawRight] = -127;
		;
	}
}



// Automatic lift function: takes three parameters, telling it what mode to enter.
void automaticLift(bool autoRaiseLift, bool autoLowerLift) {
	// These statements stop the lift once it has reached its desired height
	if (autoRaiseLift || autoLowerLift){
		if (SensorValue[leftIME] > leftIntegratedEncoderMaxValue){
			autoRaiseLift = false;
			if (SensorValue[sensor_Lift] < leftIntegratedEncoderMinValue){
				autoLowerLift = false;
			}
			if (autoRaiseLift == false && autoLowerLift == false){
				stopLift();
				liftAutoMode = false;
			}
		}
	}

	// Moves the lift
	if (autoRaiseLift) {
		raiseLift();
	}
	if (autoLowerLift) {
		lowerLift();
	}
}

void percentLift(int percent){
	float target = (percent / 100.0) * leftIntegratedEncoderMaxValue;
	clearEncoders();
	bool toOpen = true;
	if (SensorValue[leftIME] > target){
		lowerLift();
		while (SensorValue[leftIME] > target){
		}
		stopLift();
	}
	else if (SensorValue[leftIME] < target){
		raiseLift();
		while (SensorValue[leftIME] < target){
			if (SensorValue[leftIME] > 700 && toOpen)
			{
				openClawTogether();
				toOpen = false;
			}
		}
		stopLift();
	}
}

//----LCD Functions----//
void clearLCD()
{
	clearLCDLine(0);
	clearLCDLine(1);
}

//Allows you to cyle and select lcd modes
task driverControlViewValues()
{
	clearLCD();
	while (true)
	{
		if (nLCDButtons == 1)
		{
			driverControlModeCount--;
			clearLCD();
			while(nLCDButtons == 1)
			{
				wait1Msec(10);
			}
		}
		if (nLCDButtons == 4)
		{
			driverControlModeCount++;
			clearLCD();
			while(nLCDButtons == 4)
			{
				wait1Msec(10);
			}
		}
		if (driverControlModeCount < 1)
		{
			driverControlModeCount = 12;
		}
		else if (driverControlModeCount > 12)
		{
			driverControlModeCount = 1;
		}
		if (driverControlModeCount == 1)
		{
			displayLCDCenteredString(0, "Empty");
		}
		else if (driverControlModeCount == 2)
		{
			displayLCDCenteredString(0, "Currently");
			displayLCDCenteredString(1, "Empty");
		}
		else if (driverControlModeCount == 3)
		{
			displayLCDCenteredString(0, "Currently");
			displayLCDCenteredString(1, "Empty");
		}
		else if (driverControlModeCount == 4)
		{
			displayLCDCenteredString(0, "Currently");
			displayLCDCenteredString(1, "Empty");
		}
		else if (driverControlModeCount == 5)
		{
			displayLCDCenteredString(0, "Currently");
			displayLCDCenteredString(1, "Empty");
		}
		else if (driverControlModeCount == 6)
		{
			displayLCDCenteredString(0, "Currently");
			displayLCDCenteredString(1, "Empty");
		}
		else if (driverControlModeCount == 7)
		{
			displayLCDCenteredString(0, "Currently");
			displayLCDCenteredString(1, "Empty");
		}
		else if (driverControlModeCount == 8)
		{
			displayLCDCenteredString(0, "Currently");
			displayLCDCenteredString(1, "Empty");
		}
		wait1Msec(10);
	}
}

//---LCD Sensor Values---//

//Left claw potentiometer
void leftPotValue()
{
	SensorValue[leftClawPot] = 0;
	displayLCDCenteredString(0, "Left Pot");
	displayLCDNumber(1, 5, SensorValue[leftClawPot], 6);
}

//Right claw potentiometer
void rightPotValue()
{
	SensorValue[rightClawPot] = 0;
	displayLCDCenteredString(0, "Right pot");
	displayLCDNumber(1, 5, SensorValue[rightClawPot], 6);
}

//Lift quad encoder
void liftSensorValue()
{
	displayLCDCenteredString(0, "Lift quad");
	displayLCDNumber(1, 5, SensorValue[sensor_Lift], 6);
}

//Top left drive train quad encoder
void topLeftBaseValue()
{
	displayLCDCenteredString(0, "Top Left Quad");
	displayLCDNumber(1, 5, SensorValue[leftFront], 6);
}

//Back left drive train quad encoder
void backLeftBaseValue()
{
	displayLCDCenteredString(0, "Back Left Quad");
	displayLCDNumber(1, 5, SensorValue[leftBack], 6);
}

//Top right drive trian quad encoder
void topRightBaseValue()
{
	displayLCDCenteredString(0, "Front Right Quad");
	displayLCDNumber(1, 5, SensorValue[rightFront], 6);
}

//Back right drive train quad encoder
void backRightBaseValue()
{
	displayLCDCenteredString(0, "Back Right Quad");
	displayLCDNumber(1, 5, SensorValue[rightBack], 6);
}

//Top right lift integrated motor encoder
void topRightIME()
{
	displayLCDCenteredString(0, "right IME");
	displayLCDNumber(1, 5, SensorValue[rightIME], 6);
}

//Top left lift integrated motor encoder
void topLeftIME()
{
	displayLCDCenteredString(0, "left IME");
	displayLCDNumber(1, 5, SensorValue[leftIME], 6);
}

//Displays the different autos and different modes
void displayAuton(){
	switch(auton){
	case 1:
		displayLCDCenteredString(0, "right cube");
		break;
	case 2:
		displayLCDCenteredString(0, "right star");
		break;
	case 3:
		displayLCDCenteredString(0, "left cube");
		break;
	case 4:
		displayLCDCenteredString(0, "left star");
		break;
	case 5:
		leftPotValue();
		break;
	case 6:
		rightPotValue();
		break;
	case 7:
		liftSensorValue();
		break;
	case 9:
		topRightIME();
		break;
	case 10:
		topLeftIME();
		break;
	case 11:
		topLeftBaseValue();
		break;
	case 12:
		topRightBaseValue();
		break;
	case 13:
		backLeftBaseValue();
		break;
	case 14:
		backRightBaseValue();
		break;
	case 15:
		displayLCDCenteredString(0, "Right Cube Star");
		break;
	case 16:
		displayLCDCenteredString(0, "Left Cube Star");
		break;
	default:
		displayLCDCenteredString(0, "no auto");
	}
}

//Sets each buttons function
task LCDControl()
{
	clearLCDLine(0);
	clearLCDLine(1);
	bool noButtonsPressed = true;
	displayAuton();
	while(true)
	{
		if(noButtonsPressed)
		{
			switch(nLCDButtons){
			case kButtonLeft:
				auton--;
				displayAuton();
				break;
			case kButtonCenter:
				stopTask(LCDControl);
				break;
			case kButtonRight:
				auton++;
				displayAuton();
				break;
			}
		}
		noButtonsPressed = !nLCDButtons; //update if there is a button currently pressed
		wait1Msec(20);
	}
}

//---Autonomous Selection---//

//right cube auto
void rightCube()
{
driveForwardInches(3);
	openClawTogether();
	wait1Msec(333);
	percentLift(0);
	int distanceToCube = (sqrt(13) * 12) - 6;
	driveForwardInches(distanceToCube);
	wait1Msec(333);
	closeClawCube();
	percentLift(20);
	int turn = 115;
	degreeTurn(turn, left);
	wait1Msec(333);
	driveBackwardInches(17);
	wait1Msec(333);
	percentLift(100);
	wait1Msec(333);
	percentLift(0);
}

//right star auto
void rightStar()
{
	openClawTogether();
	percentLift(5);
	int starDistance = 30;
	driveForwardInches(starDistance);
	closeClawTogether();
	driveBackwardInches(starDistance);
	degreeTurn(90, left);
	driveForwardInches(24);
	degreeTurn(180, right);
	driveBackwardInches(24);
	percentLift(100);
	openClawTogether();
	percentLift(10);
}

//right cube and star auto
void rightCubeStar()
{
	driveForwardInches(3);
	openClawTogether();
	wait1Msec(333);
	percentLift(0);
	int distanceToCube = (sqrt(13) * 12) - 6;
	driveForwardInches(distanceToCube);
	wait1Msec(333);
	closeClawCube();
	percentLift(20);
	int turn = 115;
	degreeTurn(turn, left);
	wait1Msec(333);
	driveBackwardInches(17);
	wait1Msec(333);
	percentLift(100);
	wait1Msec(333);
	percentLift(0);
	openClawTogether();
	driveForwardInches(36);
	closeClaw();
	wait1Msec(750);
	stopClaw();
	driveBackwardInches(36);
	percentLift(100);
	wait1Msec(333);
	percentLift(5);
}

//left cube auto
void leftCube()
{
	driveForwardInches(3);
	openClawTogether();
	wait1Msec(333);
	percentLift(0);
	int distanceToCube = (sqrt(13) * 12) - 6;
	driveForwardInches(distanceToCube);
	wait1Msec(333);
	closeClawCube();
	percentLift(20);
	int turn = 115;
	degreeTurn(turn, right);
	wait1Msec(333);
	driveBackwardInches(17);
	wait1Msec(333);
	percentLift(100);
	wait1Msec(333);
	percentLift(0);
}

//left star auto
void leftStar()
{
	openClawTogether();
	percentLift(5);
	int starDistance = 30;
	driveForwardInches(starDistance);
	closeClawTogether();
	driveBackwardInches(starDistance);
	degreeTurn(90, right);
	driveForwardInches(24);
	degreeTurn(180, left);
	driveBackwardInches(24);
	percentLift(100);
	openClawTogether();
	percentLift(10);
}

///left cube and star auto
void leftCubeStar()
{
	driveForwardInches(3);
	openClawTogether();
	wait1Msec(333);
	percentLift(0);
	int distanceToCube = (sqrt(13) * 12) - 6;
	driveForwardInches(distanceToCube);
	wait1Msec(333);
	closeClawCube();
	percentLift(20);
	int turn = 115;
	degreeTurn(turn, right);
	wait1Msec(333);
	driveBackwardInches(17);
	wait1Msec(333);
	percentLift(100);
	wait1Msec(333);
	percentLift(0);
	openClawTogether();
	driveForwardInches(36);
	closeClaw();
	wait1Msec(750);
	stopClaw();
	driveBackwardInches(36);
	percentLift(100);
	wait1Msec(333);
	percentLift(5);
}

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;
	//autoSelection();
	bDisplayCompetitionStatusOnLcd = false;
	bLCDBacklight = true;
	startTask(LCDControl);
}


//-------------Autonomous Code-------------//


task autonomous()
{
	stopTask(LCDControl);
	stopTask(driverControlViewValues);
	playSoundFile("sound.wav");
	switch(auton)
	{
	case 1:
		rightCube();
		break;
	case 2:
		rightStar();
		break;
	case 3:
		leftCube();
		break;
	case 4:
		leftStar();
		break;
	case 15:
		rightCubeStar();
		break;
	case 16:
		leftCubeStar();
		break;
	default:
		break;
	}
}


//-------------User Control Code-------------//

task usercontrol()
{
	bLCDBacklight = true;
	string mainBattery, backupBattery;
	stopTask(LCDControl);
	while (true)
	{
		//Displays battery voltage
		clearLCDLine(0);
		clearLCDLine(1);
		//Primary voltage
		displayLCDString(0, 0, "Primary:");
		sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0, 'V');
		displayNextLCDString(mainBattery);
		//Backup voltage
		displayLCDString(1, 0, "Backup:");
		sprintf(backupBattery, "%1.2f%c", BackupBatteryLevel/1000.0, 'V');
		displayNextLCDString(backupBattery);

		/*		// Enables reverse mode
		if (vexRT[Btn7D] == 1 && !b8DOld) {
		if (reverser == false){
		reverser = true;
		}
		else if (reverser){
		reverser = false;
		}
		b7DOld = true;
		}

		if (vexRT[Btn7D] == 0 && b7DOld){
		b7DOld = false;
		}*/

		if (vexRT[Btn7D] == 1)
		{
			closeClawTogether();
		}

		//Create "deadzone" for Y1/Ch2
		if(abs(vexRT[Ch2]) > threshold)
			Y1 = -vexRT[Ch2];
		else
			Y1 = 0;
		//Create "deadzone" for Y2/Ch3
		if(abs(vexRT[Ch3]) > threshold)
			Y2 = vexRT[Ch3];
		else
			Y2 = 0;

		//Remote Control Commands

		if (reverser){ // Reverses the controls
			motor[rightDT] = -Y2;
			motor[leftDT] = Y1;
		}
		if (!reverser){
			motor[rightDT] = -Y1;
			motor[leftDT] = Y2;
		}


		//Remote Control Commands

		if (reverser){ // Reverses the controls
			motor[rightDT] = -Y2;
			motor[leftDT] = Y1;
		}
		if (!reverser){
			motor[rightDT] = -Y1;
			motor[leftDT] = Y2;
		}

		//---Manual Lift Control--//

		// Disable the automatic lift if any manual lift input detected
		if (vexRT[Btn6D] == 1 || vexRT[Btn6U] == 1 || vexRT[Btn8D] == 1){
			liftAutoMode = false;
		}

		// Raise lift
		if (vexRT[Btn6U] == 1) {
			raiseLift();
		}

		// Lower lift
		if (vexRT[Btn6D] == 1) {
			lowerLift();
		}

		// Manually disable the lift
		if (vexRT[Btn6U] == 0 && vexRT[Btn6D] == 0 && !liftAutoMode) {
			stopLift();
		}

		// Claw
		if (vexRT[Btn5U] == 1){
			openClaw();
		}
		if (vexRT[Btn5D] == 1){
			closeClaw2();
		}
		if (vexRT[Btn5D] == 0 && vexRT[Btn5U] == 0 && !autoClaw){
			stopClaw();
		}
		if (vexRT[Btn7U] == 1)
		{
			autoClaw = true;
			openClawTogether();
		}


		//--- Automatic Lift Control---//

		// Enables automatic raise to top
		if (vexRT[Btn7R] == 1 && !autoRaiseLift){
			autoLowerLift = false;
			autoRaiseLift = true;
			liftAutoMode = true;
		}

		// Enables automatic lower to bottom
		if (vexRT[Btn7L] == true && !autoLowerLift) {
			autoRaiseLift = false;
			autoLowerLift = true;
			liftAutoMode = true;
		}

		// Execute the automatic raise
		if (liftAutoMode){
			automaticLift(autoRaiseLift, autoLowerLift);
		}
		// Turns off the automatic lift
		if ((autoLowerLift || autoRaiseLift) && !liftAutoMode){
			stopLift();
		}
	}
}
