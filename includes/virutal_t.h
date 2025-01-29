#ifndef VIRTUAL_T // Si MI_ARCHIVO_H no está definido
#define VIRTUAL_T // Defínelo ahora

// Aquí van las declaraciones, definiciones y prototipos
#define BMP_TYPE 0x4d42
#define HEIGHT_TERM  24
#define WIDTH_TERM   80
#define BI_RGB 0
#define WIDTH_CHAR 8
#define HEIGHT_CHAR 16
#pragma pack(push, 1) 
typedef struct
{
  unsigned short  bfType; // 2 byte
  unsigned int    bfSize; // 4 bytes
  unsigned short  bfReserved1; // 2 bytes
  unsigned short  bfReserved2; // 2 bytes
  unsigned int    bfOffBits; // 4 bytes
  // total de esta estrucgtura 14 bytes
}BMPFILEHEADER;

typedef struct
{
  unsigned long  biSize;  //4 bytes
  long  biWidth; // 4 bytes
  long  biHeight; // 4 bytes
  unsigned short  biPlanes; //2 bytes
  unsigned short  biBitCount; //2
  unsigned long biCompression;//4
  unsigned long  biSizeImage;//4
  long  biXPelsPerMeter; //4
  long  biYPelsPerMeter; //4
  unsigned long biClrUsed; //4
  unsigned long biClrImportant; //4 total = 42 bytes
  // despues de esta estrucutra biene la paleta si es que la tiene
}BMPINFOHEADER;
#pragma pack(pop)     

typedef struct{
  unsigned char  rgbBlue;
  unsigned char  rgbGreen;
  unsigned char  rgbRed;
  unsigned char  rgbReserved;
  // total 4 bytes
}RGB;

typedef struct
{
  unsigned char blue;
  unsigned char green;
  unsigned char red;
}BGR;

typedef struct
{
  BMPFILEHEADER BmpFile;
  BMPINFOHEADER BmpInfo;
  RGB *Rgb;
  //ESTA ESTRUCTURA SE USARÁ PRA CREAR UN NUEVO ARCHIVO BMP
  
}BMPFILE;
typedef struct
{
 
  long   bmHeight; // esto es la altura en pixeles
  long   bmPitch; // este es el ancho pero en bytes o la sancada
  unsigned short   bmBitsPixel; // bits por pixel
  void * bmBits; // puntero al primer pixele del bmp
  RGB  * palete; // puntero a la paleta  de colores  si tiene null no esta
  const char * name;
}BMP;

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;BMP UTILITIES;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
void load_bmp(BMP *Bmp, const char * path);
void flip_bmp(void * pixel, unsigned int dataType, unsigned int width, unsigned int height);
void save_bmp(BMP *Bmp,const char* name);
BMP create_bmp(const char *name, BGR * Color,unsigned int widthInPixel,unsigned int heightInPixel,unsigned char bytePerPixel);
/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; VIRTUAL TERMINAL;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/



typedef struct
{
  void * ptrBmpChar; // apunta al  primer caracter
  unsigned char asciiCode;
  unsigned char color;
  unsigned char width;
  unsigned char height;
}Cell;
typedef struct
{
  unsigned int x;
  unsigned int y;
  
}Point;
typedef struct
{
  unsigned char * ptrStr; // puntero a una cadena
  unsigned int leng; // longitud de la cadena sin incluir el cero de terminacion
}Line;


Vterm *inic_virtualt(void *ScreenBuffer, int Width, int Height);
void free_console(Vterm * consol);
void load_arrascii(Cell *arr,BMP *fontSheet);

void render_ascii(Cell *ascii,unsigned int x,
		  unsigned int y,
		  void * ptrScreen,
		  unsigned int pitchFontSheet,
		  unsigned int pitchScreen);
void add_line(Line * line, const char * str, unsigned int len);

void render_line(Line *line,
		 Cell *arr,
		 unsigned char *screenBuffer,
		 unsigned int screenPitch,
		 unsigned int fontSheetPitch);

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/



#endif // Fin del include guard
