#include <ctype.h>
#include <stdio.h>
#include "..\..\Vt\includes\keyboard.h"

extern KeyboardEvent keyEvent;
void debugKeyboard(KeyboardEvent k)
{
  char buffer[BUTTON_COUNT][100];
  for(int x=0; x< BUTTON_COUNT; x++)
    {
      if(1/*((k.orderButtons[x].isDown == 1) && (k.buttons[x].changed == 1))*/)
      	{
	  sprintf(buffer[x],"Keyboard estate\n"
		  "VK:\t%d\n"
		  "ORDINAL:\t%d\n"
		  "ISDOWN:\t%d\n"
		  "CHANGED:\t%d\n--------------------------------",k.buttons[x].vk,
		  k.buttons[x].ordinal,
		  k.buttons[x].isDown,
		  k.buttons[x].changed);
	  OutputDebugStringA(buffer[x]);			  
	   }
}
}


int ispressedAnyKey()
{
  int flag = 0;
    for (int i = 1; i < BUTTON_COUNT; i++) {
        if (keyEvent.buttons[i].isDown && keyEvent.buttons[i].changed) {
            flag = 1;
            break;
        }
    }
    return flag;
    
}
int isdownAnyKey()
{
  int flag = 0;
    for (int i = 1; i < BUTTON_COUNT; i++) {
        if (keyEvent.buttons[i].isDown) {
            flag = 1;
            break;
        }
    }
    return flag;
}
int ispressedCtrlAt(Button b)
{
  return (((b.vk == BUTTON_UP)) &&( b.isDown) && (b.changed)||
	  ((b.vk == BUTTON_DOWN)) &&( b.isDown) && (b.changed)||
	  ((b.vk == BUTTON_RIGHT)) &&( b.isDown) && (b.changed)||
	  ((b.vk == BUTTON_LEFT)) &&( b.isDown) && (b.changed)||
	  // ((b.vk == BUTTON_RETURN)) &&( b.isDown) && (b.changed)||
	  //  ((b.vk == BUTTON_BACK)) &&( b.isDown) && (b.changed)||
	  ((b.vk == BUTTON_CTRL)) &&( b.isDown) && (b.changed)); 
}
int isdownCtrlAt(Button b)
{
  return (((b.vk == BUTTON_UP)) &&( b.isDown)||
	  ((b.vk == BUTTON_DOWN)) &&( b.isDown)||
	  ((b.vk == BUTTON_RIGHT)) &&( b.isDown)||
	  ((b.vk == BUTTON_LEFT)) &&( b.isDown)||
	  ((b.vk == BUTTON_SHIFT)) &&( b.isDown)||
	  //((b.vk == BUTTON_BACK)) &&( b.isDown)||
	  ((b.vk == BUTTON_CTRL)) &&( b.isDown));
  
}

int ispressedAlphaAt(Button b)
{
  return ((isalpha(b.vk)) && (b.isDown) && (b.changed));
  
}
int isdownAlphaAt(Button b)
{
  return ((isalpha(b.vk)) && (b.isDown));
  
}

int ispressedArrowKeyAt(Button b)
{
  return(((b.vk == BUTTON_UP)&&(b.isDown && b.changed))||
   ((b.vk == BUTTON_DOWN)&&(b.isDown && b.changed))||
   ((b.vk == BUTTON_RIGHT)&&(b.isDown && b.changed))||
   ((b.vk == BUTTON_LEFT)&&(b.isDown && b.changed)));
}
int isdownArrowKeyAt(Button b)
{
   return(((b.vk == BUTTON_UP)&&(b.isDown))||
   ((b.vk == BUTTON_DOWN)&&(b.isDown))||
   ((b.vk == BUTTON_RIGHT)&&(b.isDown))||
   ((b.vk == BUTTON_LEFT)&&(b.isDown)));
}
// verifica si se preciono una tecla que contiene un caracter imprimible
ispressedPrintable(Button b)
{
  return ((isprint(b.vk)) && (b.isDown) && (b.changed));
  
}
int ispressedAt(Button b, int button)
{
  return ((b.vk == button) && (b.isDown && b.changed)); 
  
}
int downAt(Button b, int button)
{
  return ((b.vk == button) && (b.isDown)); 
}

int ispressedOemButton(Button b)
{
  int result=0;
  if(((b.vk >= BUTTON_OEM_1) && (b.vk <= BUTTON_OEM_102)))
    {
      if((b.isDown) && (b.changed))
	{
	  result=1; 
	}
    }
  return result;
}






