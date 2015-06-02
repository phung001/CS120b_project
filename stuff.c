
static char menu;
static char cursor;
static char button_layout[8];
static double notes[16];
static double song1[];
static double song2[];
static double song3[];
static double song4[];
static char button_hand;
static char button_shake;
unsigned char TimerFlag = 0;
void TimerISR() {
   TimerFlag = 1;
}


enum Keypad_States { Keypad_init, Keypad_wait, Keypad_down, Keypad_up, Keypad_enter/back } Keypad_State;

TickFct_Keypad_movement() {
   //MAKE FUNCTION TO FIND THE POSITION ACCORDING TO KEY
   switch(Keypad_State) { // Transitions
      case -1:
         Keypad_State = Keypad_init;
         break;
      case Keypad_init:
         if (1) {
            Keypad_State = Keypad_wait;
         }
         break;
      case Keypad_wait:
         if (keypad == 8 && cursor > 0) {
            Keypad_State = Keypad_up;
         }
         else if (keypad == 5 && cursor < 3) {
            Keypad_State = Keypad_down;
         }
         else if (keypad != 8 || keypad != 5 //else) {
            Keypad_State = Keypad_wait;
         }
         else if (keypad == 2 || keypad == 0) {
            Keypad_State = Keypad_enter/back;
         }
         break;
      case Keypad_down:
         if (shake != 0) {
            Keypad_State = Keypad_down;
         }
         else if (shake == 0) {
            Keypad_State = Keypad_wait;
         }
         break;
      case Keypad_up:
         if (shake == 0) {
            Keypad_State = Keypad_wait;
         }
         else if (shake != 0) {
            Keypad_State = Keypad_up;
         }
         break;
      case Keypad_enter/back:
         if (shake == 0) {
            Keypad_State = Keypad_wait;
         }
         else if (shake != 0) {
            Keypad_State = Keypad_enter/back;
         }
         break;
      default:
         Keypad_State = Keypad_init;
      } // Transitions

   switch(Keypad_State) { // State actions
      case Keypad_init:
         hand = 0;
         break;
      case Keypad_wait:
         hand = 0;
         break;
      case Keypad_down:
         hand = 1;
         cursor++;
         break;
      case Keypad_up:
         hand = 1;
         cursor --;
         break;
      case Keypad_enter/back:
         hand = 1;
         cursor = 0;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

enum Menu_States { Menu_init, Menu_button_wait, Menu_main_wait, Menu_record_wait, Menu_play_wait, Menu_reset_wait, Menu_reset, Menu_song1_play, Menu_song2_play, Menu_song3_play, Menu_song4_play, Menu_chang_button_wait, Menu_song1_record, Menu_song2_record, Menu_song3_record, Menu_song4_record } Menu_State;

TickFct_Menu() {
   string main_disp[5];
string song_disp[5];
   switch(Menu_State) { // Transitions
      case -1:
         Menu_State = Menu_init;
         break;
      case Menu_init:
         if (1) {
            Menu_State = Menu_main_wait;
         }
         break;
      case Menu_button_wait:
         if (keypad != 2 //else) {
            Menu_State = Menu_button_wait;
         }
         else if (keypad == 0) {
            Menu_State = Menu_main_wait;
         }
         else if (keypad == 2 //enter) {
            Menu_State = Menu_chang_button_wait;
         }
         break;
      case Menu_main_wait:
         if (cursor == 2 && keypad == 2) {
            Menu_State = Menu_button_wait;
         }
         else if (cursor == 1 && keypad == 2) {
            Menu_State = Menu_record_wait;
         }
         else if (cursor == 0 && keypad == 2) {
            Menu_State = Menu_play_wait;
         }
         else if (cursor == 3 && keypad == 2) {
            Menu_State = Menu_reset_wait;
         }
         else if (//else) {
            Menu_State = Menu_main_wait;
         }
         break;
      case Menu_record_wait:
         if (keypad == 0) {
            Menu_State = Menu_main_wait;
         }
         else if (cursor == 0 && keypad == 2) {
            Menu_State = Menu_song1_record;
         }
         else if (cursor == 1 && keypad == 2) {
            Menu_State = Menu_song2_record;
         }
         else if (cursor == 2 && keypad == 2) {
            Menu_State = Menu_song3_record;
         }
         else if (cursor == 3 && keypad == 2) {
            Menu_State = Menu_song4_record;
         }
         else if (//else) {
            Menu_State = Menu_record_wait;
         }
         break;
      case Menu_play_wait:
         if (keypad == 0) {
            Menu_State = Menu_main_wait;
         }
         else if (//else) {
            Menu_State = Menu_play_wait;
         }
         else if (cursor == 0 && keypad == 2) {
            Menu_State = Menu_song1_play;
         }
         else if (cursor == 1 && keypad == 2) {
            Menu_State = Menu_song2_play;
         }
         else if (cursor == 2 && keypad == 2) {
            Menu_State = Menu_song3_play;
         }
         else if (cursor == 3 && keypad == 2) {
            Menu_State = Menu_song4_play;
         }
         break;
      case Menu_reset_wait:
         if (keypad == 0) {
            Menu_State = Menu_main_wait;
         }
         else if (//else) {
            Menu_State = Menu_reset_wait;
         }
         else if (keypad == 2) {
            Menu_State = Menu_reset;
         }
         break;
      case Menu_reset:
         if (1) {
            Menu_State = Menu_main_wait;
         }
         break;
      case Menu_song1_play:
         if (keypad == 0) {
            Menu_State = Menu_play_wait;
         }
         break;
      case Menu_song2_play:
         if (keypad == 0) {
            Menu_State = Menu_play_wait;
         }
         break;
      case Menu_song3_play:
         if (keypad == 0) {
            Menu_State = Menu_play_wait;
         }
         break;
      case Menu_song4_play:
         if (keypad == 0) {
            Menu_State = Menu_play_wait;
         }
         break;
      case Menu_chang_button_wait:
         if (button_shake == 0) {
            Menu_State = Menu_button_wait;
         }
         else if (button_shake != 0;) {
            Menu_State = Menu_chang_button_wait;
         }
         break;
      case Menu_song1_record:
         if (keypad == 0) {
            Menu_State = Menu_record_wait;
         }
         break;
      case Menu_song2_record:
         if (keypad == 0) {
            Menu_State = Menu_record_wait;
         }
         break;
      case Menu_song3_record:
         if (keypad == 0) {
            Menu_State = Menu_record_wait;
         }
         break;
      case Menu_song4_record:
         if (keypad == 0) {
            Menu_State = Menu_record_wait;
         }
         break;
      default:
         Menu_State = Menu_init;
      } // Transitions

   switch(Menu_State) { // State actions
      case Menu_init:
         menu = 0;
         break;
      case Menu_button_wait:
         menu = 3;
         button_hand = 0;
         display(button instructions)
         break;
      case Menu_main_wait:
         menu = 0;
         display(main_disp[cursor]);
         break;
      case Menu_record_wait:
         menu = 2;
         display(song_disp[cursor])
         break;
      case Menu_play_wait:
         menu = 1;
         display(song_disp[cursor])
         break;
      case Menu_reset_wait:
         menu = 4;
         display(are you sure)
         break;
      case Menu_reset:
         //do the reset
         break;
      case Menu_song1_play:
         //play the song
         display(song name)
         break;
      case Menu_song2_play:
         //play the song
         display(song name)
         break;
      case Menu_song3_play:
         //play the song
         display(song name)
         break;
      case Menu_song4_play:
         //play the song
         display(song name)
         break;
      case Menu_chang_button_wait:
         //do the chang_button state machine
         button_hand = 1;
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
}

enum Button_States { Button_init, Button_wait, Button_change, Button_note_up, Button_note_down } Button_State;

TickFct_button_change() {
   static char cap_but; //holds the note[] position of the button
static char position; //holds the button_layout[] position of the button
   switch(Button_State) { // Transitions
      case -1:
         Button_State = Button_init;
         break;
      case Button_init:
         if (1) {
            Button_State = Button_wait;
         }
         break;
      case Button_wait:
         if ((PinC6 || PinC7) && button_hand == 1
) {
            Button_State = Button_change;
            button_shake = 1;
position = posi(key);
cap_but = button_layout[position];
         }
         else if (else) {
            Button_State = Button_wait;
         }
         else if (keypad == 0) {
            Button_State = Button_wait;
            button_shake = 0;
         }
         break;
      case Button_change:
         if (keypad == 8 && cap_but < 16) {
            Button_State = Button_note_up;
            cap_but++;
         }
         else if (keypad == 5 && cap_but > 0) {
            Button_State = Button_note_down;
            cap_but--;
         }
         else if (keypad == 0
) {
            Button_State = Button_wait;
            PWM(0);
button_shake = 0;
         }
         else if (keypad == 2) {
            Button_State = Button_wait;
            PWM(0);
button_layout[position] = cap_but;
button_shake = 0;
         }
         break;
      case Button_note_up:
         if (1) {
            Button_State = Button_change;
         }
         else if (keypad == 8) {
            Button_State = Button_note_up;
         }
         break;
      case Button_note_down:
         if (1) {
            Button_State = Button_change;
         }
         else if (keypad == 5) {
            Button_State = Button_note_down;
         }
         break;
      default:
         Button_State = Button_init;
      } // Transitions

   switch(Button_State) { // State actions
      case Button_init:
         button_shake = 0;
         break;
      case Button_wait:
         break;
      case Button_change:
         PWM(notes[cap_but])
         break;
      case Button_note_up:
         break;
      case Button_note_down:
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
int main() {
   B = 0; //Init outputs
   TimerSet(1000);
   TimerOn();
   Keypad_State = -1;
   Menu_State = -1;
   Button_State = -1;
   while(1) {
      TickFct_Keypad_movement();
      TickFct_Menu();
      TickFct_button_change();
      while (!TimerFlag);
      TimerFlag = 0;
   }
}
