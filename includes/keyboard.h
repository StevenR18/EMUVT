#ifndef KEY_BOARD_H
#define KEY_BOARD_H
typedef struct
{
  int vk;
  int processed;
  int ordinal; // indica en que orden llegan los eventos
  int isDown;  // indica si la telca fue presionada
  int changed; // indica si ubo un cambio
}Button;

enum {
    BUTTON_CTRL          = 0x11,  // VK_CTRL
    BUTTON_LEFT          = 0x25,  // VK_LEFT
    BUTTON_RIGHT         = 0x27, // VK_RIGHT
    BUTTON_UP            = 0x26,    // VK_UP
    BUTTON_DOWN          = 0x28,  // VK_DOWN
    BUTTON_W             = 0x57,     // 'W'
    BUTTON_A             = 0x41,     // 'A'
    BUTTON_S             = 0x53,     // 'S'
    BUTTON_D             = 0x44,     // 'D'
    BUTTON_BS            = 0x20,
    BUTTON_SHIFT         = 0x10,
    BUTTON_OEM_1         = 0xBA,	
    BUTTON_OEM_PLUS      = 0xBB,	
    BUTTON_OEM_COMMA     = 0xBC,	
    BUTTON_OEM_MINUS     = 0xBD,	
    BUTTON_OEM_PERIOD    = 0xBE,	
    BUTTON_OEM_2         = 0xBF,	
    BUTTON_OEM_3         = 0xC0,	
    BUTTON_OEM_4         = 0xDB,	
    BUTTON_OEM_5         = 0xDC,	
    BUTTON_OEM_6         = 0xDD,	
    BUTTON_OEM_7         = 0xDE,	
    BUTTON_OEM_8         = 0xDF,	
    BUTTON_OEM_102       = 0xE2,
    BUTTON_RETURN        = 0x0D,
    BUTTON_BACK          = 0x8,
    BUTTON_COUNT =256
};

typedef struct
{
  Button buttons[BUTTON_COUNT];
  Button orderButtons[BUTTON_COUNT]; // orden de botones segun llegada
  char keystate[BUTTON_COUNT];
}KeyboardEvent;

int ispressedAnyKey();
void debugKeyboard(KeyboardEvent k);
int ispressedCtrlAt(Button b);
int ispressedAlphaAt(Button b);
int isdownAlphaAt(Button b);
int ispressedArrowKeyAt(Button b);
int isdownArrowKeyAt(Button b);
int downAt(Button b, int button);
int downAnyKey();
int isdownCtrlAt(Button b);
int ispressedOemButton(Button b);

#define pressed(b)(\
		   keyEvent.buttons[b].isDown\
		   &&\
		   keyEvent.buttons[b].changed)
 
#define released(b)(\
		   !keyEvent.buttons[b].isDown\
		   &&\
		   keyEvent.buttons[b].changed)

#define down(b)(keyEvent.buttons[b].isDown)

#define ispressedArrowKey (\
			   keyEvent.buttons[BUTTON_LEFT]\
			   ||\
			   keyEvent.buttons[BUTTON_RIGHT]\
			   ||\
			   keyEvent.buttons[BUTTON_DOWN]\
			   ||\
			   keyEvent.buttons[BUTTON_UP])


				
#endif

