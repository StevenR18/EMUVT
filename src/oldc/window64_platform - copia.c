#include <stdio.h>
#include <string.h>
#include <Windows.h>
#define STRUCT_INIC(W) memset((char *)(W),0,sizeof((*W)))
#define CLASS_NAME "Class_Window64"
#define WINDOW_NAME "DattaMetter"
static HINSTANCE GHInst;
static void *GPixels=NULL;
static BITMAPINFO GPBmi;
static int Runnig=1;
#include "..\Vt\includes\window64_platform.h"
static Win32ScreenBuffer GBuffer;
static KeyBoard GkeyBoard;
//#include "..\Vt\includes\virtual_t.h"
#include "..\Vt\includes\main_datametter.h"
/*********************GETER*******************************************************/
Win32ScreenBuffer * getWindow32ScreenBuffer()
{
  return &GBuffer;
}
/**************************************************************************************/



void Win32ClsScreen(Win32ScreenBuffer *Buffer, unsigned int Color)
{
  int Width= Buffer->Width;
  int Height= Buffer->Height;
  int SizeBuffer= (Width*Height);
  unsigned int *FirstPixel= (unsigned int *)Buffer->Pixels;
 
  for(int x=0; x<SizeBuffer; x++)
      FirstPixel[x]=Color;
}


void Win32RenderBuffer(HDC GHdc,
		      int WindowWidth,
		      int WindowHeight,
		      Win32ScreenBuffer *Buffer)
{

  StretchDIBits(GHdc,
			     0,/*x destino*/
			     0,/*y destino*/
			     WindowWidth,/*Destino ancho*/
			     WindowHeight,/*Destino alto*/
			     0, /*x origen*/
			     0, /*y origne*/
			     Buffer->Width,/*ancho origen*/
			     Buffer->Height,/*alto origen*/
			     Buffer->Pixels, /* void * pixeles*/
			     &Buffer->Info,/*const BITMAPINFO *lpbmi*/
			     DIB_RGB_COLORS,
			     SRCCOPY);


  Win32ClsScreen(Buffer,0);

  
}

void Win32ResizeDibSection(Win32ScreenBuffer *Buffer,
			   int Width,
			   int Height)
{

  if(Buffer->Pixels)
    {
      VirtualFree(Buffer->Pixels,0,MEM_RELEASE);
    }
  Buffer->Width=Width;
  Buffer->Height=Height;
  int BytesPerPixel= sizeof(int); //4 bytes
  STRUCT_INIC(&Buffer->Info);
  // Llenar la estructura bitmap info
  Buffer->Info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  Buffer->Info.bmiHeader.biWidth=Width;
  Buffer->Info.bmiHeader.biHeight= -Height;
  Buffer->Info.bmiHeader.biPlanes=1;
  Buffer->Info.bmiHeader.biBitCount=32;
  Buffer->Info.bmiHeader.biCompression=BI_RGB;
   /*RESERVAR MEMEMORIA PARA LOS PIXELES*/
  int BitmapMemorySize= Buffer->Width *Buffer->Height * BytesPerPixel;
  Buffer->Pixels=VirtualAlloc(0,
			      BitmapMemorySize,
			      MEM_RESERVE|MEM_COMMIT,
			      PAGE_READWRITE);

  Buffer->Pitch= Buffer->Width*BytesPerPixel;
  
}

Win32WindowDimension Win32GetWindowDimension(HWND Window )
{
  Win32WindowDimension Result;
  RECT ClientRect;
  GetClientRect(Window,&ClientRect);
  Result.Width=ClientRect.right - ClientRect.left;
  Result.Height=ClientRect.bottom -ClientRect.top;
  return Result;
}


/*****************************PROCEDIMIENTO DE LA VENTANA*******************************/
LRESULT CALLBACK MainWndProc(
    HWND HWindow,        // handle to window
    UINT UMsg,        // message identifier
    WPARAM WParam,    // first message parameter
    LPARAM LParam)    // second message parameter
{
   LRESULT Result=0;
  switch(UMsg)
    {
    case WM_CHAR:
      {
	GkeyBoard.AsciiCode= WParam;
        GkeyBoard.KeyState = LParam;
      }break;
    case WM_CREATE:
      {
	 OutputDebugStringA("WM_CREATE\n");
	 
      }break;
    case WM_PAINT:
      {
	/*Este es el sgundo mensaje que se procesa*/
	 OutputDebugStringA("WM_PAINT\n");
	 /*se deve procesar el mensaje por lo menos una vez
	  para indicarle a windows que la patalla a sido pintada
	 y no vuelva a pedir que se pinte*/
         PAINTSTRUCT Paintstr; // region que necesita ser redibujada
	 HDC Hdc= BeginPaint(HWindow,&Paintstr); // devuelve un hdc con la region a acutaliar
	  Win32WindowDimension Dimension=Win32GetWindowDimension(HWindow);
	  Win32ResizeDibSection(&GBuffer,Dimension.Width,Dimension.Height);
	  
	  Win32RenderBuffer(Hdc,
			   Dimension.Width,
		           Dimension.Height,
			   &GBuffer);	 
	 EndPaint(HWindow, &Paintstr); // libera el contexto y valida la region apintar
	                            // esto hace que no se envie mensajes del WM_PAINT
                                    // de forma indefinida
      }break;
    case WM_SIZE:
      {
	 OutputDebugStringA("WM_SIZE\n");
	
      }break;
    case WM_DESTROY:
      {
	OutputDebugStringA("WM_DESTROY.\n");
	PostQuitMessage(0);	
      }break;
    default:
      Result=DefWindowProc(HWindow,UMsg,WParam,LParam);
    }
 
return Result;
 
}
/***************************************************************************************/
int WINAPI WinMain(HINSTANCE HInst,
		   HINSTANCE HInstPrev,
		   PSTR Pszcmdline,
		   int Ncmdshow)
{

  GHInst= HInst;
  Win32ResizeDibSection(&GBuffer,1280,720);
  WNDCLASSEXA WindowClass;
  STRUCT_INIC(&WindowClass);
  WindowClass.cbSize= sizeof(WNDCLASSEXA);
  WindowClass.style=CS_HREDRAW|CS_OWNDC|CS_VREDRAW;
  WindowClass.lpfnWndProc=MainWndProc;
  WindowClass.hInstance= HInst;
  WindowClass.hIcon=NULL;
  WindowClass.hCursor=NULL;
  WindowClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
  WindowClass.lpszMenuName=NULL;
  WindowClass.lpszClassName=CLASS_NAME;
  WindowClass.hIconSm=NULL;

  

  if(RegisterClassExA(&WindowClass))
    {
      OutputDebugStringA("SE REGISTRO CORRECTAMENTE WindowClassS\n");
      HWND HWindow= CreateWindowExA(0,
				    CLASS_NAME,
				    WINDOW_NAME,
				    WS_VISIBLE|WS_OVERLAPPEDWINDOW,
				    CW_USEDEFAULT ,/*CORDENADA X DE LA VENTAN*/
				    CW_USEDEFAULT, /*CORDENADA Y DE LA VENTANA*/
				    CW_USEDEFAULT,/*ANCHO DE LA VENTANA*/
				    CW_USEDEFAULT,/*ALTO DE LA VENTANA*/
				    NULL,         /*MANEJADOR DE LA VENTANA PADRE*/
				    NULL,
				    HInst,
				    NULL);

      if(HWindow)
	{
	   OutputDebugStringA("SE CREO CORRECTAMENTE LA VENTANA\n");
	   inic_app();
	   HDC GHdc= GetDC(HWindow);
	   while(Runnig)
	     {
	       MSG Message;
	      
	       while(PeekMessage(&Message,0,0,0,PM_REMOVE))// nos permite vaciar la cola
		 {
		   if(Message.message == WM_QUIT)
		     {
		       Runnig=0;
		     }
		    TranslateMessage(&Message);
                    DispatchMessageA(&Message);	     		    
		 }
	       

	       Win32WindowDimension Dimension = Win32GetWindowDimension(HWindow);
	       Win32RenderBuffer(GHdc,Dimension.Width, Dimension.Height,&GBuffer);
	       main_app(&GBuffer,&HWindow,&GkeyBoard);
	       /// inicio de la aplicacion
	       STRUCT_INIC(&GkeyBoard);
	       Sleep(16);
	     }
	   DeleteDc(Ghdc);
	}
      else
	{
	   OutputDebugStringA("FALLO LA CREACION DE LA VENTANA");

	}

    }
  else
    {
       OutputDebugStringA("FALLÓ EL REGISTRO DE LA CLASE WindowClass");
    }


  return 0;
}


