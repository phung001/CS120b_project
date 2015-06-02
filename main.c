#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "./path/io.c"
#include "./path/io.h"

//-------------------------------------------------------------------------------------
// macro for easier usage
#define read_eeprom_word(address) eeprom_read_word ((const uint16_t*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((uint16_t*)address,(uint16_t)value)
#define update_eeprom_word(address,value) eeprom_update_word ((uint16_t*)address,(uint16_t)value)

//declare an eeprom array
unsigned char EEMEM  my_eeprom_array;

// declare a ram array
unsigned char my_ram_array;

//----------------------------------------------------------------------------------------

double notes[16] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88,
523.25, 587.33, 659.25, 698.46, 783.99, 880.00, 987.77, 1046.50, 0};

unsigned char buttons[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	
unsigned char button_hand;
unsigned char button_shake;
unsigned char cursor = '0';
unsigned char key;
/*
double song1[50] = { ,1.23}; //1 terminated for each song
double song2[50] = { ,1.23};
double song3[50] = { ,1.23};
double song4[50] = { ,1.23};
*/

//--------Task scheduler data structure--------------------
// Struct for Tasks represent a running process in our
// simple real-time operating system.
/*Tasks should have members that include: state, period, a
measurement of elapsed time, and a function pointer.*/
typedef struct _task {
	//Task's current state, period, and the time elapsed
	// since the last tick
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	//Task tick function
	int (*TickFct)(int);
} task;
task tasks[3];
const unsigned char numTasks=3;

enum keypad_states{SM_1};

int tick_getkeys(int state)
{
	key=GetKeypadKey();
	return state;
}

/*enum States{dairy};
int tick_scroll(int state)
{
	LCD_Cursor(1);
	if(key) {
		LCD_WriteData(key);
		my_ram_array++;
		update_eeprom_word(&my_eeprom_array, my_ram_array);
	}
	LCD_Cursor(2);
	my_ram_array = read_eeprom_word(&my_eeprom_array);
	LCD_WriteData(my_ram_array);
	
	return state;
	
}*/
//-----------------------------------------------------------------------------------------------

/*
static char button_layout[8];
static double song1[];
static double song2[];
static double song3[];
static double song4[];
*/


//-----------------------------------------------------------------------------------------------
enum Keypad_States { Keypad_init, Keypad_wait, Keypad_down, Keypad_up, Keypad_enter_back } Keypad_State;

int Keypad_movement(int state) {
	
	switch(state) { // Transitions
		case -1:
		state = Keypad_init;
		break;
		case Keypad_init:
		if (1) {
			state = Keypad_wait;
		}
		break;
		case Keypad_wait:
		if (key == '8' && cursor > '0') {
			state = Keypad_up;
			cursor--;
		}
		else if (key == '5' && cursor < '3') {
			state = Keypad_down;
			cursor++;
		}
		else if (key == '2' || key == '0') {
			state = Keypad_enter_back;
			//cursor = '0';
		}
		else{
			state = Keypad_wait;
		}
		break;
		case Keypad_down:
		if (key == '5') {
			state = Keypad_down;
		}
		else{
			state = Keypad_wait;
		}
		break;
		case Keypad_up:
		if (key == '8') {
			state = Keypad_up;
		}
		else{
			state = Keypad_wait;
		}
		break;
		case Keypad_enter_back:
		if (key == '2' || key == '0') {
			state = Keypad_enter_back;
		}
		else{
			state = Keypad_wait;
		}
		break;
		default:
		state = Keypad_init;
	} // Transitions

	switch(state) { // State actions
		case Keypad_init:
//		LCD_DisplayString(1, "init");
		cursor = '0';
		break;
		case Keypad_wait:
//		LCD_DisplayString(1, "wait");
		break;
		case Keypad_down:
//		LCD_DisplayString(1, "down");
		break;
		case Keypad_up:
//		LCD_DisplayString(1, "up");
		break;
		case Keypad_enter_back:
//		LCD_DisplayString(1, "enter/back");
		break;
		default: // ADD default behaviour below
		break;
	} // State actions

//	LCD_Cursor(11);
//	LCD_WriteData(cursor+'0');
	return state;
}

//-----------------------------------------------------------------------------------------------
enum Menu_States { Menu_init, Menu_button_wait, Menu_main_wait, Menu_record_wait, Menu_play_wait, Menu_reset_wait, Menu_reset, Menu_song1_play, Menu_song2_play, Menu_song3_play, Menu_song4_play, Menu_chang_button_wait, Menu_song1_record, Menu_song2_record, Menu_song3_record, Menu_song4_record } Menu_State;
int Menu(int state1) {

//	string main_disp[5];
//	string song_disp[5];

	switch(state1) { // Transitions
		case -1:
		state1 = Menu_init;
		break;
		case Menu_init:
		if (1) {
			state1 = Menu_main_wait;
		}
		break;
		case Menu_button_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			cursor = '0';
		}
		else if (key == '2') {
			state1 = Menu_chang_button_wait;
		}
		else{ //else
			state1 = Menu_button_wait;
		}
		break;
		case Menu_main_wait:
		if ((cursor == '2') && (key == '2')) {
			state1 = Menu_button_wait;
			cursor = '0';
		}
		else if ((cursor == '1') && (key == '2')) {
			state1 = Menu_record_wait;
			cursor = '0';
		}
		else if ((cursor == '0') && (key == '2')) {
			state1 = Menu_play_wait;
			cursor = '0';
		}
		else if ((cursor == '3') && (key == '2')) {
			state1 = Menu_reset_wait;
			cursor = '0';
		}
		else{
			state1 = Menu_main_wait;
		}
		break;
		case Menu_record_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			cursor = '0';
		}
		else if (cursor == '0' && key == '2') {
			state1 = Menu_song1_record;
		}
		else if (cursor == '1' && key == '2') {
			state1 = Menu_song2_record;
		}
		else if (cursor == '2' && key == '2') {
			state1 = Menu_song3_record;
		}
		else if (cursor == '3' && key == '2') {
			state1 = Menu_song4_record;
		}
		else{
			state1 = Menu_record_wait;
		}
		break;
		case Menu_play_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			cursor = '0';
		}
		else if (cursor == '0' && key == '2') {
			state1 = Menu_song1_play;
		}
		else if (cursor == '1' && key == '2') {
			state1 = Menu_song2_play;
		}
		else if (cursor == '2' && key == '2') {
			state1 = Menu_song3_play;
		}
		else if (cursor == '3' && key == '2') {
			state1 = Menu_song4_play;
		}
		else{
			state1 = Menu_play_wait;
		}
		break;
		case Menu_reset_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			cursor = '0';
		}
		else if (key == '2') {
			state1 = Menu_reset;
		}
		else{
			state1 = Menu_reset_wait;
		}
		break;
		case Menu_reset:
		if (1) {
			state1 = Menu_main_wait;
		}
		break;
		case Menu_song1_play:
		if (key == '0') {
			state1 = Menu_play_wait;
		}
		break;
		case Menu_song2_play:
		if (key == '0') {
			state1 = Menu_play_wait;
		}
		break;
		case Menu_song3_play:
		if (key == '0') {
			state1 = Menu_play_wait;
		}
		break;
		case Menu_song4_play:
		if (key == '0') {
			state1 = Menu_play_wait;
		}
		break;
		case Menu_chang_button_wait:
		if (button_shake == 0) {
			state1 = Menu_button_wait;
		}
		else if (button_shake != 0) {
			state1 = Menu_chang_button_wait;
		}
		break;
		case Menu_song1_record:
		if (key == '0') {
			state1 = Menu_record_wait;
		}
		break;
		case Menu_song2_record:
		if (key == '0') {
			state1 = Menu_record_wait;
		}
		break;
		case Menu_song3_record:
		if (key == '0') {
			state1 = Menu_record_wait;
		}
		break;
		case Menu_song4_record:
		if (key == '0') {
			state1 = Menu_record_wait;
		}
		break;
		default:
		state1 = Menu_init;
	} // Transitions

	switch(state1) { // State actions
		case Menu_init:
		LCD_DisplayString(1, "button menu section");
		break;
		case Menu_button_wait:
		button_hand = 0;
		LCD_DisplayString(1, "button menu section");
		break;
		case Menu_main_wait:
		LCD_DisplayString(1, "main menu section");
		break;
		case Menu_record_wait:
		LCD_DisplayString(1, "record menu section");
		break;
		case Menu_play_wait:
		LCD_DisplayString(1, "play menu section");
		break;
		case Menu_reset_wait:
		LCD_DisplayString(1, "reset menu section");
		break;
		case Menu_reset:
		LCD_DisplayString(1, "reset go section");
		//do the reset
		break;
		case Menu_song1_play:
		//play the song
		LCD_DisplayString(1, "play song1");
		break;
		case Menu_song2_play:
		//play the song
		LCD_DisplayString(1, "play song2");
		break;
		case Menu_song3_play:
		//play the song
		LCD_DisplayString(1, "play song3");
		break;
		case Menu_song4_play:
		//play the song
		LCD_DisplayString(1, "play song4");
		break;
		case Menu_chang_button_wait:
		//do the chang_button state machine
		LCD_DisplayString(1, "change button go");
		button_hand = 1;
		break;
		case Menu_song1_record:
		LCD_DisplayString(1, "record song1");
		break;
		case Menu_song2_record:
		LCD_DisplayString(1, "record song2");
		break;
		case Menu_song3_record:
		LCD_DisplayString(1, "record song3");
		break;
		case Menu_song4_record:
		LCD_DisplayString(1, "record song4");
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	return state1;
} 
/*
//-----------------------------------------------------------------------------------------------
enum Button_States { Button_init, Button_wait, Button_change, Button_note_up, Button_note_down } Button_State;
int button_change(int state) {

	static char cap_but; //holds the note[] position of the button
	static char position; //holds the button_layout[] position of the button

	switch(state) { // Transitions
		case -1:
		state = Button_init;
		break;
		case Button_init:
		if (1) {
			state = Button_wait;
		}
		break;
		case Button_wait:
		if ((position = GetButton()) && button_hand == 1) {
			state = Button_change;
			button_shake = 1;
			position = button_position(position);
			cap_but = note_position(position, buttons);
		}
		else if (key == 0) {
			state = Button_wait;
			button_shake = 0;
		}
		else{
			state = Button_wait;
		}
		break;
		case Button_change:
		if (key == 8 && cap_but < 16) {
			state = Button_note_up;
			cap_but++;
		}
		else if (key == 5 && cap_but > 0) {
			state = Button_note_down;
			cap_but--;
		}
		else if (key == 0) {
			state = Button_wait;
			set_PWM(0);
			button_shake = 0;
		}
		else if (key == 2) {
			Button_State = Button_wait;
			set_PWM(0);
			buttons[position] = cap_but;
			button_shake = 0;
		}
		else{
			state = Button_change;
		}
		break;
		case Button_note_up:
		if (key != 8) {
			state = Button_change;
		}
		else if (key == 8) {
			state = Button_note_up;
		}
		break;
		case Button_note_down:
		if (key != 5) {
			state = Button_change;
		}
		else if (key == 5) {
			state = Button_note_down;
		}
		break;
		default:
		state = Button_init;
	} // Transitions

	switch(state) { // State actions
		case Button_init:
		button_shake = 0;
		break;
		case Button_wait:
		break;
		case Button_change:
		TimerSet(100);		
		set_PWM(notes[cap_but]);
		while (!TimerFlag){}
		TimerFlag = 0;
		TimerSet(GCD);
		break;
		case Button_note_up:
		break;
		case Button_note_down:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions	
	return state;
} */
//-----------------------------------------------------------------------------------------------

// --------END User defined FSMs---------------------------// Implement scheduler code from PES.
int main(void)
{
	unsigned char i=0;
	tasks[i].state = SM_1;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &tick_getkeys;
	++i;
	
	tasks[i].state = Keypad_init;
	tasks[i].period = 4;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &Keypad_movement;
	++i;
	
	tasks[i].state = Menu_init;
	tasks[i].period = 3;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &Menu;
	++i;
	/*
	tasks[i].state = Button_init;
	tasks[i].period = 4;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &button_change;
	*/
	DDRA = 0xFF; PORTA = 0x00; // LCD
	//DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00; // LCD
	DDRC = 0xF0; PORTC = 0x0F; //keypad
	LCD_init();
	// Starting at position 1 on the LCD screen
	//LCD_DisplayString(1, "button menu section");
	
	PWM_on();
	TimerOn();
	
	TimerSet(50);
	TimerOn();
	/*
	LCD_Cursor(2);
	my_ram_array = read_eeprom_word(&my_eeprom_array);
	LCD_WriteData(my_ram_array);
	*/
	while(1)
	{
		for (i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i].elapsedTime ==
			tasks[i].period ) {
				// Setting next state for task
				tasks[i].state =
				tasks[i].TickFct(tasks[i].state);
				// Reset elapsed time for next tick.
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += 1;
		}
		
		LCD_Cursor(31);
		LCD_WriteData(cursor);
		LCD_Cursor(32);
		LCD_WriteData(key);
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
