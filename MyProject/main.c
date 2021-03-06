#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>
#include "sensors/proximity.h"
#include "motors.h"
#include "leds.h"
#include "spi_comm.h"
#include "sensors/VL53L0X/VL53L0X.h"
#include "selector.h"
#include "audio/audio_thread.h"
#include "audio/play_melody.h"
#include "audio/play_sound_file.h"

void Task1();
void Task2();

//For task 1
int maxVel = 1200; //maximum velocity
int distance = 200; //distance between e-puck2 and objectives

//For task 2
int StopDis = 200; //the distance that e-puck2 needs to stop
int maxDis = 600; //the maximum distance that e-puck2 could detect sth

int main(void)
{
    halInit();
    chSysInit();
    mpu_init();
    clear_leds();
    spi_comm_start();
    proximity_start();
    calibrate_ir();
    motors_init();

    /* Infinite loop. */
    while (1) {
    	if(get_selector() == 0){
    		left_motor_set_speed(0);
    		right_motor_set_speed(0);
    	}
    	else if(get_selector() == 1){
    		Task1();
    	}
    	else if(get_selector() == 2){
    	    Task2();
    	}

    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

void Task1(){
	//initial speed
	left_motor_set_speed(900);
	right_motor_set_speed(1000);

	//when obstacles are on the left, turn right
	if(get_prox(7) < distance || get_prox(6) < distance || get_prox(5) < distance){
	    left_motor_set_speed(700);
	    right_motor_set_speed(-500);
	}

	//when obstacles are on the right, turn left
	if(get_prox(0) < distance || get_prox(1) < distance || get_prox(2) < distance){
	    left_motor_set_speed(-500);
	    right_motor_set_speed(700);
	}

}

void Task2(){
	//e-puck2 arrived around the object, stop to avoid collision
	if(get_prox(1) < StopDis
			|| get_prox(2) < StopDis
			|| get_prox(3) < StopDis
			|| get_prox(4) < StopDis
			|| get_prox(5) < StopDis
			|| get_prox(6) < StopDis
			|| get_prox(7) < StopDis
			|| get_prox(8) < StopDis){

	    	left_motor_set_speed(0);
	    	right_motor_set_speed(0);

	    	//when the object is detected, the body led toggles
	    	set_body_led(2);
	}

	if((get_prox(0) > StopDis && get_prox(0) < maxDis)
			|| (get_prox(7) > StopDis && get_prox(7) < maxDis)){
		//object is detected by sensor 0 or 7, go forward
	    left_motor_set_speed(1000);
	    right_motor_set_speed(1000);
	}
	else if((get_prox(1) > StopDis && get_prox(1) < maxDis)
			|| (get_prox(2) > StopDis && get_prox(2) < maxDis)){
		//object is detected by sensor 1 or 2, turn right
	    left_motor_set_speed(800);
	    right_motor_set_speed(-700);
	}
	else if((get_prox(5) > StopDis && get_prox(5) < maxDis)
			|| (get_prox(6) > StopDis && get_prox(6) < maxDis)){
		//object is detected by sensor 5 or 6, turn left
		left_motor_set_speed(-700);
		right_motor_set_speed(800);
	}
	else if((get_prox(3) > StopDis && get_prox(3) < maxDis)
			|| (get_prox(4) > StopDis && get_prox(4) < maxDis)){
		//object is detected by sensor 3 or 4, turn around
	    left_motor_set_speed(700);
	    right_motor_set_speed(-700);
	}
	else if(get_prox(1) > maxDis
			|| get_prox(2) > maxDis
			|| get_prox(3) > maxDis
			|| get_prox(4) > maxDis
			|| get_prox(5) > maxDis
			|| get_prox(6) > maxDis
			|| get_prox(7) > maxDis
			|| get_prox(8) > maxDis){
		//nothing is detected, go forward
		left_motor_set_speed(900);
		right_motor_set_speed(1000);
	}
}
