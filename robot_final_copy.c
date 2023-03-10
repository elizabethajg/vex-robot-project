#pragma config(Sensor, in1,    ir_sensor,      sensorReflection)
#pragma config(Sensor, dgtl1,  y_min,          sensorTouch)
#pragma config(Sensor, dgtl2,  y_max,          sensorTouch)
#pragma config(Sensor, dgtl3,  x_min,          sensorTouch)
#pragma config(Sensor, dgtl4,  x_max,          sensorTouch)
#pragma config(Sensor, dgtl6,  count_msb,      sensorLEDtoVCC)
#pragma config(Sensor, dgtl7,  count_led4,     sensorLEDtoVCC)
#pragma config(Sensor, dgtl8,  count_led3,     sensorLEDtoVCC)
#pragma config(Sensor, dgtl9,  count_led2,     sensorLEDtoVCC)
#pragma config(Sensor, dgtl10, count_lsb,      sensorLEDtoVCC)
#pragma config(Sensor, dgtl11, red_led,        sensorLEDtoVCC)
#pragma config(Motor,  port2,           x_motor,       tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port3,           y_motor,       tmotorServoContinuousRotation, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Purpose: Updates the count displayed on leds
void update_leds(int count){
	if(count == 1){
		SensorValue[count_lsb] = 1;
	}
	else if(count == 2){
		SensorValue[count_lsb] = 0;
		SensorValue[count_led2] = 1;
	}
	else if(count == 3){
		SensorValue[count_lsb] = 1;
		SensorValue[count_led2] = 1;
	}
	else if(count == 4){
		SensorValue[count_lsb] = 0;
		SensorValue[count_led2] = 0;
		SensorValue[count_led3] = 1;
	}
	else if(count == 5){
		SensorValue[count_lsb] = 1;
		SensorValue[count_led2] = 0;
		SensorValue[count_led3] = 1;
	}
	else if(count == 6){
		SensorValue[count_lsb] = 0;
		SensorValue[count_led2] = 1;
		SensorValue[count_led3] = 1;
	}
	else if(count == 7){
		SensorValue[count_lsb] = 1;
		SensorValue[count_led2] = 1;
		SensorValue[count_led3] = 1;
	}
		else if(count == 8){
		SensorValue[count_lsb] = 0;
		SensorValue[count_led2] = 0;
		SensorValue[count_led3] = 0;
		SensorValue[count_led4] = 1;
	}
	else if(count == 9){
		SensorValue[count_lsb] = 1;
		SensorValue[count_led2] = 0;
		SensorValue[count_led3] = 0;
		SensorValue[count_led4] = 1;
	}
	else if(count == 10){
		SensorValue[count_lsb] = 0;
		SensorValue[count_led2] = 1;
		SensorValue[count_led3] = 0;
		SensorValue[count_led4] = 1;
	}
	else if(count == 0){
		SensorValue[count_lsb] = 0;
		SensorValue[count_led2] = 0;
		SensorValue[count_led3] = 0;
		SensorValue[count_led4] = 0;
		SensorValue[count_msb] = 0;
	}
	return;
}

//Purpose: Moves carriage to initial position
void to_start(int motor_speed){
	if(SensorValue[x_min] == 0 || SensorValue[y_max] == 0)
	{
		//move carriage to x_min position
		while(SensorValue[x_min] == 0){
			motor[x_motor] = -motor_speed;
		}
		//stop x motor
		motor[x_motor] = 0;
		//move carriage to y_min position
		while(SensorValue[y_max] == 0){
			motor[y_motor] = motor_speed;
		}
		//stop y motor
		motor[y_motor] = 0;
	}
	wait10Msec(150);
}

//Purpose: Continually detects and updates counts of black squares
void detect_squares(int* count, int* array_ptr, int ir_threshold){
	if(SensorValue[ir_sensor] > ir_threshold){
			SensorValue[red_led] = 1;
			wait10Msec(200);
			SensorValue[red_led] = 0;
			(*count)++;
			writeDebugStream("%d squares detected\n", *count);
			update_leds(*count);
			//To indicate a square was detected in this row, assign a value of 1 to the corresponding array element
			*array_ptr = 1;
	}
}

//Purpose: Moves carriage down one row
void next_row(int motor_speed, int wait_time){
	if(SensorValue[y_min] == 0){
		motor[y_motor] = -motor_speed;
		wait10Msec(wait_time);
		motor[y_motor] = 0;
	}
}

//Purpose: Resets all count sensors to 0
void clear_sensors(){
	SensorValue[red_led] = 0;
	SensorValue[count_lsb] = 0;
	SensorValue[count_led2] = 0;
	SensorValue[count_led3] = 0;
	SensorValue[count_led4] = 0;
	SensorValue[count_msb] = 0;
}

//Purpose: Initiates first scan of paper
void first_scan(int wait_time, int* array, int motor_speed, int ir_threshold){
	int row_num = 0;
	int count = 0;
	int* array_ptr = array;

	while(!(SensorValue[y_min] == 1 && SensorValue[x_min] == 1))
	{
		//If the carriage is at position x_max, move toward x_min
		if(SensorValue[x_max] == 1)
		{
			while(SensorValue[x_min] == 0)
			{
				motor[x_motor] = -motor_speed;
				detect_squares(&count, array_ptr, ir_threshold);
			}
			motor[x_motor] = 0;
		//if the carriage is at position x_min, move toward x_max
		}else if(SensorValue[x_min] == 1){
			while(SensorValue[x_max] == 0)
			{
				motor[x_motor] = motor_speed;
				detect_squares(&count, array_ptr, ir_threshold);
			}
			motor[x_motor] = 0;
		}

		//Add one to row_num so that it is printed
		row_num++;
		writeDebugStream("%d row(s) scanned\n", row_num);
		next_row(motor_speed, wait_time);
		//after moving carriage across x_axis, move ~1cm toward y_min
		for(int i; i<row_num; i++){
			writeDebugStream("%d ", array[i]);
		}
		array_ptr++;
		writeDebugStream("\n");
	}
}

void second_scan(int wait_time, int* array, int motor_speed, int ir_threshold){
	int row_num = 0;
	int count = 0;
	int* array_ptr = array;

	for(int i = 0; !(SensorValue[y_min] == 1 && SensorValue[x_min] == 1); i++)
	{
		if(array[i] == 1)
		{
			//If the carriage is at position x_max, move toward x_min
			if(SensorValue[x_max] == 1)
			{
				while(SensorValue[x_min] == 0)
				{
					motor[x_motor] = -motor_speed;
					detect_squares(&count, array_ptr, ir_threshold);
				}
				motor[x_motor] = 0;
			}else if(SensorValue[x_min] == 1){
				while(SensorValue[x_max] == 0)
				{
					motor[x_motor] = motor_speed;
					detect_squares(&count, array_ptr, ir_threshold);
				}
				motor[x_motor] = 0;
			}
		}

		row_num++;
		next_row(motor_speed, wait_time);
		//after moving carriage across x_axis, move ~1cm toward y_min
		for(int i; i<row_num; i++){
			writeDebugStream("%d ", array[i]);
		}
		array_ptr++;
		writeDebugStream("\n");
	}
}


task main()
{
	int speed = 50;
	int wait = 165;
	int ir_over_square = 2800;
	int square_detected[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	clear_sensors();
	to_start(speed);
	first_scan(wait, square_detected, speed, ir_over_square);
	wait10Msec(300);

	clear_sensors();
	to_start(speed);
	second_scan(wait, square_detected, speed, ir_over_square);

	return;
}
