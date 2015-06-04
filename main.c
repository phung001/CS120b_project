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
double EEMEM eeprom_song1[99];
double EEMEM eeprom_song2[99];
double EEMEM eeprom_song3[99];
double EEMEM eeprom_song4[99];

unsigned char EEMEM eeprom_buttons[8];
unsigned char EEMEM eeprom_check;
unsigned char check = 0;

//----------------------------------------------------------------------------------------

double notes[16] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88,
523.25, 587.33, 659.25, 698.46, 783.99, 880.00, 987.77, 1046.50, 0};

char* button_notes[16] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5", "D5", "E5", "F5", "G5", "A5", "B5", "C6", "00"};

unsigned char buttons[8] = {0, 1, 2, 3, 4, 5, 6, 7};

unsigned char button_hand;
unsigned char button_shake;
unsigned char record_hand;
unsigned char record_shake;
unsigned char cursor = '0';
unsigned char key;
unsigned char count = 0;

double song1[99] = {293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66,
	293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 293.66, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00,
392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 392.00, 1.23}; //1.23 terminated for each song
double song2[99] = {0 ,1.23};
double song3[99] = {0 ,1.23};
double song4[99] = {0 ,1.23};
double song_temp[99];
unsigned char which_song = 0;


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
task tasks[5];
const unsigned char numTasks=5;

enum keypad_states{SM_1};

int tick_getkeys(int state)
{
	key=GetKeypadKey();
	return state;
}


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

	const char*  main_disp[4] = {"-Play Songs     Record Songs", "-Record Songs   Change Button", "-Change Button  Reset All", "-Reset All"};
	const char*  song_disp[4] = {"-Song #1        Song #2", "-Song #2        Song #3", "-Song #3        Song #4", "-Song #4"};

	switch(state1) { // Transitions
		case -1:
		state1 = Menu_init;
		break;
		case Menu_init:
		if (1) {
			state1 = Menu_main_wait;
			LCD_DisplayString(1, main_disp[0]);
		}
		break;
		case Menu_button_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			LCD_DisplayString(1, main_disp[0]);
			cursor = '0';
		}
		else if (key == '2') {
			state1 = Menu_chang_button_wait;
			button_hand = 1;
		}
		else{ //else
			state1 = Menu_button_wait;
		}
		break;
		case Menu_main_wait:
		if ((cursor == '2') && (key == '2')) {
			state1 = Menu_button_wait;
			LCD_DisplayString(1, "Press enter to  change a button");
			cursor = '0';
		}
		else if ((cursor == '1') && (key == '2')) {
			state1 = Menu_record_wait;
			LCD_DisplayString(1, song_disp[0]);
			cursor = '0';
		}
		else if ((cursor == '0') && (key == '2')) {
			state1 = Menu_play_wait;
			LCD_DisplayString(1, song_disp[0]);
			cursor = '0';
		}
		else if ((cursor == '3') && (key == '2')) {
			state1 = Menu_reset_wait;
			LCD_DisplayString(1, "Press ENTER to  reset or go back");
			cursor = '0';
		}
		else if((cursor == '0') && (key == '8')){
			LCD_DisplayString(1, main_disp[0]);
			state1 = Menu_main_wait;
		}
		else if(((cursor == '1') && (key == '8')) || ((cursor == '1') && (key == '5'))){
			LCD_DisplayString(1, main_disp[1]);
			state1 = Menu_main_wait;
		}
		else if(((cursor == '2') && (key == '8')) || ((cursor == '2') && (key == '5'))){
			LCD_DisplayString(1, main_disp[2]);
			state1 = Menu_main_wait;
		}
		else if((cursor == '3') && (key == '5')){
			LCD_DisplayString(1, main_disp[3]);
			state1 = Menu_main_wait;
		}
		else{
			state1 = Menu_main_wait;
		}
		break;
		case Menu_record_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			LCD_DisplayString(1, main_disp[0]);
			cursor = '0';
		}
		else if (cursor == '0' && key == '2') {
			state1 = Menu_song1_record;
			record_hand = 1;
			which_song = 1;
		}
		else if (cursor == '1' && key == '2') {
			state1 = Menu_song2_record;
			record_hand = 1;
			which_song = 2;
		}
		else if (cursor == '2' && key == '2') {
			state1 = Menu_song3_record;
			record_hand = 1;
			which_song = 3;
		}
		else if (cursor == '3' && key == '2') {
			state1 = Menu_song4_record;
			record_hand = 1;
			which_song = 4;
		}
		else if((cursor == '0') && (key == '8')){
			LCD_DisplayString(1, song_disp[0]);
			state1 = Menu_record_wait;
		}
		else if(((cursor == '1') && (key == '8')) || ((cursor == '1') && (key == '5'))){
			LCD_DisplayString(1, song_disp[1]);
			state1 = Menu_record_wait;
		}
		else if(((cursor == '2') && (key == '8')) || ((cursor == '2') && (key == '5'))){
			LCD_DisplayString(1, song_disp[2]);
			state1 = Menu_record_wait;
		}
		else if((cursor == '3') && (key == '5')){
			LCD_DisplayString(1, song_disp[3]);
			state1 = Menu_record_wait;
		}
		else{
			state1 = Menu_record_wait;
		}
		break;
		case Menu_play_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			LCD_DisplayString(1, main_disp[0]);
			cursor = '0';
		}
		else if (cursor == '0' && key == '2') {
			state1 = Menu_song1_play;
			LCD_DisplayString(1, "Now playing:     Song 1");
			count = 0;
		}
		else if (cursor == '1' && key == '2') {
			state1 = Menu_song2_play;
			LCD_DisplayString(1, "Now playing:     Song 2");
			count = 0;
		}
		else if (cursor == '2' && key == '2') {
			state1 = Menu_song3_play;
			LCD_DisplayString(1, "Now playing:     Song 3");
			count = 0;
		}
		else if (cursor == '3' && key == '2') {
			state1 = Menu_song4_play;
			LCD_DisplayString(1, "Now playing:     Song 4");
			count = 0;
		}
		else if((cursor == '0') && (key == '8')){
			LCD_DisplayString(1, song_disp[0]);
			state1 = Menu_play_wait;
		}
		else if(((cursor == '1') && (key == '8')) || ((cursor == '1') && (key == '5'))){
			LCD_DisplayString(1, song_disp[1]);
			state1 = Menu_play_wait;
		}
		else if(((cursor == '2') && (key == '8')) || ((cursor == '2') && (key == '5'))){
			LCD_DisplayString(1, song_disp[2]);
			state1 = Menu_play_wait;
		}
		else if((cursor == '3') && (key == '5')){
			LCD_DisplayString(1, song_disp[3]);
			state1 = Menu_play_wait;
		}
		else{
			state1 = Menu_play_wait;
		}
		break;
		case Menu_reset_wait:
		if (key == '0') {
			state1 = Menu_main_wait;
			LCD_DisplayString(1, main_disp[0]);
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
			LCD_DisplayString(1, main_disp[0]);
		}
		break;
		case Menu_song1_play:
		if (key == '0' || song1[count] == 1.23 || count >= 99) {
			state1 = Menu_play_wait;
			LCD_DisplayString(1, song_disp[0]);
			set_PWM(0);
		}
		break;
		case Menu_song2_play:
		if (key == '0' || song2[count] == 1.23 || count >= 99) {
			state1 = Menu_play_wait;
			LCD_DisplayString(1, song_disp[0]);
			set_PWM(0);
		}
		break;
		case Menu_song3_play:
		if (key == '0' || song3[count] == 1.23 || count >= 99) {
			state1 = Menu_play_wait;
			LCD_DisplayString(1, song_disp[0]);
			set_PWM(0);
		}
		break;
		case Menu_song4_play:
		if (key == '0' || song4[count] == 1.23 || count >= 99) {
			state1 = Menu_play_wait;
			LCD_DisplayString(1, song_disp[0]);
			set_PWM(0);
		}
		break;
		case Menu_chang_button_wait:
		if (button_shake == 0) {
			state1 = Menu_button_wait;
			LCD_DisplayString(1, "Press enter to  change a button");
			button_hand = 0;
		}
		else if (button_shake != 0) {
			state1 = Menu_chang_button_wait;
		}
		break;
		case Menu_song1_record:
		if (record_shake == 0) {
			state1 = Menu_record_wait;
			LCD_DisplayString(1, song_disp[0]);
			record_hand = 0;
		}
		else{
			state1 = Menu_song1_record;
			record_hand = 1;
		}
		break;
		case Menu_song2_record:
		if (record_shake == 0) {
			state1 = Menu_record_wait;
			LCD_DisplayString(1, song_disp[0]);
			record_hand = 0;
		}
		else{
			state1 = Menu_song2_record;
			record_hand = 1;
		}
		break;
		case Menu_song3_record:
		if (record_shake == 0) {
			state1 = Menu_record_wait;
			LCD_DisplayString(1, song_disp[0]);
			record_hand = 0;
		}
		else{
			state1 = Menu_song3_record;
			record_hand = 1;
		}
		break;
		case Menu_song4_record:
		if (record_shake == 0) {
			state1 = Menu_record_wait;
			LCD_DisplayString(1, song_disp[0]);
			record_hand = 0;
		}
		else{
			state1 = Menu_song4_record;
			record_hand = 1;
		}
		break;
		default:
		state1 = Menu_init;
	} // Transitions

	switch(state1) { // State actions
		case Menu_init:
		button_hand = 0;
		record_hand = 0;
		break;
		case Menu_button_wait:
		break;
		case Menu_main_wait:
		break;
		case Menu_record_wait:
		break;
		case Menu_play_wait:
		break;
		case Menu_reset_wait:
		break;
		case Menu_reset:
		break;
		case Menu_song1_play:
		set_PWM(song1[count]);
		count++;
		break;
		case Menu_song2_play:
		set_PWM(song2[count]);
		count++;
		break;
		case Menu_song3_play:
		set_PWM(song3[count]);
		count++;
		break;
		case Menu_song4_play:
		set_PWM(song4[count]);
		count++;
		break;
		case Menu_chang_button_wait:
		break;
		case Menu_song1_record:
		break;
		case Menu_song2_record:
		break;
		case Menu_song3_record:
		break;
		case Menu_song4_record:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	return state1;
}

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
		if (button_hand == 1) {
			state = Button_wait;
			LCD_DisplayString(1, "Press the buttonto change");
			button_shake = 1;
		}
		else{
			state = Button_init;
		}
		break;
		case Button_wait:
		if (key == '#' || key == '9' || key == '6' || key == '3' || key == 'D' || key == 'C' || key == 'B' || key == 'A') {
			state = Button_change;
			position = button_position(key);
			cap_but = button_position(key);
			cap_but = buttons[cap_but];
			LCD_DisplayString(1, "Current note:");
			LCD_Cursor(15);
			LCD_WriteData(button_notes[cap_but][0]);
			LCD_Cursor(16);
			LCD_WriteData(button_notes[cap_but][1]);
		}
		else if (key == '0') {
			state = Button_init;
			button_shake = 0;
		}
		else{
			state = Button_wait;
		}
		break;
		case Button_change:
		if (key == '8' && cap_but > 0) {
			state = Button_note_up;
			cap_but--;
		}
		else if (key == '5' && cap_but < 15) {
			state = Button_note_down;
			cap_but++;
		}
		else if (key == '0') {
			state = Button_init;
			set_PWM(0);
			button_shake = 0;
		}
		else if (key == '2') {
			state = Button_init;
			set_PWM(0);
			buttons[position] = cap_but;
			write_eeprom_word(&eeprom_buttons[position], cap_but);
			button_shake = 0;
		}
		else{
			state = Button_change;
		}
		break;
		case Button_note_up:
		if (key != '8') {
			state = Button_change;
			LCD_DisplayString(1, "Current note:");
			LCD_Cursor(15);
			LCD_WriteData(button_notes[cap_but][0]);
			LCD_Cursor(16);
			LCD_WriteData(button_notes[cap_but][1]);
		}
		else if (key == '8') {
			state = Button_note_up;
		}
		break;
		case Button_note_down:
		if (key != '5') {
			state = Button_change;
			LCD_DisplayString(1, "Current note:");
			LCD_Cursor(15);
			LCD_WriteData(button_notes[cap_but][0]);
			LCD_Cursor(16);
			LCD_WriteData(button_notes[cap_but][1]);
		}
		else if (key == '5') {
			state = Button_note_down;
		}
		break;
		default:
		state = Button_init;
	} // Transitions

	switch(state) { // State actions
		case Button_init:
		break;
		case Button_wait:
		break;
		case Button_change:
		set_PWM(notes[cap_but]);
		break;
		case Button_note_up:
		break;
		case Button_note_down:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	return state;
}
//-----------------------------------------------------------------------------------------------

enum Record_States { Record_init, Record_wait, Record_go } Record_State;
int Record_change(int state) {

	switch(state) { // Transitions
		case -1:
		state = Record_init;
		break;
		case Record_init:
		if (record_hand == 1){
			state = Record_wait;
			LCD_DisplayString(1, "Press a note to start.");
			record_shake = 1;
		}
		else{
			state = Record_init;
		}
		break;
		case Record_wait:
		if(key == '#' || key == '9' || key == '6' || key == '3' || key == 'D' || key == 'C' || key == 'B' || key == 'A'){
			state = Record_go;
			LCD_DisplayString(1, "**Recording**");
		}
		else if(key == '0'){
			state = Record_init;
			record_shake = 0;
		}
		else{
			state = Record_wait;
		}
		break;
		case Record_go:
		if(count >= 98){
			state = Record_init;
			set_PWM(0);
			record_shake = 0;
			int pp = 0;
			while (pp < 98){
				if (which_song == 1) {
					song1[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song1[pp], song_temp[pp]);
				}
				else if (which_song == 2) {
					song2[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song2[pp], song_temp[pp]);
				}
				else if (which_song == 3) {
					song3[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song3[pp], song_temp[pp]);
				}
				else if (which_song == 4) {
					song4[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song4[pp], song_temp[pp]);
				}
				pp++;
			}
		}
		else if(key == '2'){
			state = Record_init;
			set_PWM(0);
			record_shake = 0;
			int pp = 0;
			song_temp[count+1] = 1.23;
			while (pp < 99){
				if (which_song == 1) {
					song1[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song1[pp], song_temp[pp]);
				}
				else if (which_song == 2) {
					song2[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song2[pp], song_temp[pp]);
				}
				else if (which_song == 3) {
					song3[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song3[pp], song_temp[pp]);
				}
				else if (which_song == 4) {
					song4[pp] = song_temp[pp];
					write_eeprom_word(&eeprom_song4[pp], song_temp[pp]);
				}
				pp++;
			}
		}
		else if(key == '0'){
			state = Record_wait;
			LCD_DisplayString(1, "Press a note to start.");
		}
		else{
			if(key == '#' || key == '9' || key == '6' || key == '3' || key == 'D' || key == 'C' || key == 'B' || key == 'A'){
				song_temp[count] = notes[buttons[button_position(key)]];
				set_PWM(song_temp[count]);
			}
			else{
				song_temp[count] = 0;
				set_PWM(0);
			}
			state = Record_go;
		}
		break;
		default:
		state = Record_init;
		break;
	} // Transitions

	switch(state) { // State actions
		case Record_init:
		break;
		case Record_wait:
		count = 0;
		break;
		case Record_go:
		count++;
		break;
		
		default: // ADD default behaviour below
		break;
	} // State actions
	return state;
}
//-----------------------------------------------------------------------------------------------

// --------END User defined FSMs---------------------------// Implement scheduler code from PES.
int main(void)
{
	unsigned char i=0;
	tasks[i].state = SM_1;
	tasks[i].period = 2;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &tick_getkeys;
	++i;
	
	tasks[i].state = Keypad_init;
	tasks[i].period = 2;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &Keypad_movement;
	++i;
	
	tasks[i].state = Menu_init;
	tasks[i].period = 4;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &Menu;
	++i;

	tasks[i].state = Button_init;
	tasks[i].period = 4;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &button_change;
	++i;

	tasks[i].state = Record_init;
	tasks[i].period = 4;
	tasks[i].elapsedTime = 1;
	tasks[i].TickFct = &Record_change;

	DDRA = 0xFF; PORTA = 0x00; // LCD
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00; // LCD
	DDRC = 0xF0; PORTC = 0x0F; //keypad
	LCD_init();
	// Starting at position 1 on the LCD screen
	
	check = read_eeprom_word(&eeprom_check);
	if(check != 1){
		write_eeprom_word(&eeprom_check, 1);
		check = 1;
		
		int j = 0;
		while(j < 8){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			write_eeprom_word(&eeprom_buttons[j], buttons[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			write_eeprom_word(&eeprom_song1[j], song1[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			write_eeprom_word(&eeprom_song2[j], song2[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			write_eeprom_word(&eeprom_song3[j], song3[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			write_eeprom_word(&eeprom_song4[j], song4[j]);
			j++;
		}
	}
	else{
		int j = 0;
		while(j < 8){
			//update_eeprom_word(&eeprom_buttons[j], buttons[j]);
			buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			song1[j] = read_eeprom_word(&eeprom_song1[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			song2[j] = read_eeprom_word(&eeprom_song2[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			song3[j] = read_eeprom_word(&eeprom_song3[j]);
			j++;
		}
		j = 0;
		while(j < 98){
			//buttons[j] = read_eeprom_word(&eeprom_buttons[j]);
			song4[j] = read_eeprom_word(&eeprom_song4[j]);
			j++;
		}
	}
	
	PWM_on();
	TimerOn();
	
	TimerSet(25);
	TimerOn();

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
		}/*
		LCD_Cursor(32);
		LCD_WriteData(key);
		LCD_Cursor(31);
		LCD_WriteData(cursor);
*/
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
