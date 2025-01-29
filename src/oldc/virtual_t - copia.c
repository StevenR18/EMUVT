#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define BMP_TYPE 0x4d42
#define HEIGHT_TERM  24
#define WIDTH_TERM   80
#define BI_RGB 0
#pragma pack(push, 1) 
/*
  que tan dificil puede ser porgramar un emulador de terminal
  desde cero usando solo c ???


 */
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
  /// estructura principla de la terminal
  
}Vterm;
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



int main()
{
  #define WIDTH_CHAR 8
  #define HEIGHT_CHAR 16

    Point Cursor={.x=0,.y=0}; // POSICION DEL CURSOR
    BMP Screen={0};
    BGR Color={0x00,0x00,0x00}; // color negro para la pantalla de la terminal
    Screen= create_bmp("screen.bmp",&Color,400,400,3);
    int Width= Screen.bmPitch/4;
    int Height= Screen.bmHeight;
    Line **arrLine=NULL; /// AY QUE RESERVAR MEORIA
    // buffer de linias
    int col = 400/8; // maximo de celdas en la pantalla
    int row = 400/16; // maximo de filas en la pantalla
    arrLine= (Line **) malloc(sizeof(Line *) * row);
    for(int x=0; x< row; x++)
      {
	arrLine[x]= (Line *) malloc(sizeof(Line)*col);
      }

    Line  line={.ptrStr=NULL,.leng=0};
    //PTRLINES ->    * PTRLINE->     [CHAR *|INT]
    //                        0X10             0X55         
    BMP fontSheet={0};
    load_bmp(&fontSheet,"font.bmp");
    Cell arrASCIIchar[256];
    /// cargar los caracteres
    load_arrascii(arrASCIIchar,&fontSheet);
  

    // TODO:
    // *crear un buffer de caracteres dinamico
    // * otra funcion que permita renderizar  el buffer de caracteres
    // crear la api de plataforma para poder renderizar en tiempo real
    
    char *Cadena="\x1b[0;0HPERROS";
    add_line(&line,Cadena,strlen(Cadena));
      
    // add_line(&line,"Mundo",5);

    // rederiza una linia en la pantalla
    render_line(&line,
		 arrASCIIchar,
		 Screen.bmBits,
		 Screen.bmPitch,
		fontSheet.bmPitch);

   
    

    
    save_bmp(&Screen,"screen.bmp");
    // crear funciones  de dibujo de formas
    
    //liberar los punteros que se crearon con malloc 
  
  return 0;
}

/*;;;;;;;;;;;;;;;;;;Funciones de la terminal virtual;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
void load_arrascii(Cell *arr, BMP *fontSheet)
{

    for(int row=0; row < 8; row++)
      {
	for(int col=0; col < 32; col++)
	  {
	    unsigned int index= ((row *(HEIGHT_CHAR )) *(fontSheet->bmPitch/4) + (col*(WIDTH_CHAR+1)));
	    unsigned int indexArr=row * 32 + col;
	   unsigned int * ptrPixelFont = (unsigned int *)fontSheet->bmBits;
	   arr[indexArr].ptrBmpChar=(void *)&ptrPixelFont[index];
	   arr[indexArr].asciiCode= (row*32+col);
	   arr[indexArr].color=0;
	   arr[indexArr].width=WIDTH_CHAR;
	   arr[indexArr].height=HEIGHT_CHAR;
	   
	  }
      }
}


void render_ascii(Cell *ascii,unsigned int x,
		  unsigned int y,
		  void * bmBits,
		  unsigned int pitchFontSheet,
		  unsigned int pitchScreen)
{
  // TODO
  // COMPROBAR SI EL CARACTER TIENE COLOR
  // CAMBIAR DE COLOR

   for(int rowChar=0; rowChar < HEIGHT_CHAR; rowChar++)
	      {
		// rowchar 1
		int px= x*8;  //0
		int py= y*(HEIGHT_CHAR);
		
		int destOfset= (rowChar+py) * pitchScreen + (px*4);//0
		int srcOfset = (rowChar) * pitchFontSheet ;//0
		unsigned char *dest = bmBits;
                unsigned char *src = (unsigned char *) ascii->ptrBmpChar;
		memcpy(&dest[destOfset], &src[srcOfset], WIDTH_CHAR * 4);
		
		
		
	      }
  
}

void add_line(Line * line, const char * str, unsigned int len)
{
 
  unsigned int newSize=0;
  if(line->ptrStr == NULL)
    newSize= len;
  else
    newSize= line->leng+len;
   
  line->ptrStr=(unsigned char *)realloc(line->ptrStr, newSize+1);
  memcpy(&(line->ptrStr[(line->leng)]), str, len);
  line->leng+=len;
  line->ptrStr[line->leng]='\0';
}

/*
  render_line : Recibe un buffer de linias que en errealidad es un estrucutra
                 con un buffer de caracteres, [a,e,b,\n,ESC] debe ser capas
		 de renderizar los caracteres imprimibles y actuar de acuerdo
		 a las secuencias de escape moviendo el cursor, haciendo saltos de linea
		 retornos de caro, cambiar de color los caracrteres,limpiar la pantalla,

 */

void render_line(Line *line,
		 Cell *arr,
		 unsigned char *screenBuffer,
		 unsigned int screenPitch,
		 unsigned int fontSheetPitch)
{
  int maxCell=line->leng;
  static Point cursor={.x=0,.y=0};// inicializacion del cursor
  static Point oldCursor={.x=0,.y=0};
  for(int cell=0; cell < maxCell; cell++)
    {
      unsigned char code= line->ptrStr[cell];
      if(code >= 32 && code  <= 126 )// rango ASCII que son imprimibles
	{
	  if(cursor.x >= 44)
	    {
	      cursor.x=1;
	      cursor.y++;
	    }
	   
	render_ascii(&arr[code],
		 cursor.x, // x
		 cursor.y, // y
		 (unsigned char *)screenBuffer,
		 fontSheetPitch, // sancada de la hoja de sprites
		 screenPitch); // sancada de la pantalla
	cursor.x++;
	        
         }
      else
	{
	  if(code == '\n')cursor.y++;
	  if(code == '\r')cursor.x=0;
	  // procesar las secuencias de escape
	  if(code ==0x1b) //ESC caracter de inicio de una secuencias
	    {
	      cell++; // abanzamos una poscion en el buffer de caracteres
	      code = line->ptrStr[cell];
	      if(code == 'M')
		{
		  // realiza la operacion inversa \n
		  cursor.y--;
		}
	      if(code == 7)
		{
		  // permite guardar la posicion del cursor en memoria
		  oldCursor=cursor;
		}
	      if (code == 8)
		{
		  // permite restaurar la poscion del cursor en memoria.
		  cursor=oldCursor;
		  
		}
	      if(code == '[') // inicio de un secuencia de escape con parametros
		{
		  cell++; // digito; digito A
		  unsigned char params[5]={0,0,0,0,0};
		  unsigned int countParam=0;
		  unsigned int lenDigitY=0;
		  unsigned int lenDigitX=0;
		  unsigned int flagParam=1; // nos dice si cambio al prametro x
		  while((isdigit(line->ptrStr[cell]))||(line->ptrStr[cell] == ';'))
		    {
		     
		      
		      if(isdigit(line->ptrStr[cell]))
			{
			  // char * validParam;
			  char * digit = (char *)&line->ptrStr[cell];
			  //int number= strtol((const char *)&line->ptrStr[cell], &validParam, 10);
			 
			      // el buffer contiene un numero valido
			       params[countParam]=*digit;
			       lenDigitY+=((flagParam == 1) ? 1 : 0);
			       lenDigitX+=((flagParam == 0) ? 1 : 0); 
			       countParam++;

			}
		      else flagParam=((line->ptrStr[cell] == ';') ? 0 : 1);		      	
		      cell++;
		      
		    }
		   unsigned char finalChar= line->ptrStr[cell];
		   // gurada lo parametros si los tiene 
		   int paramY=0;
		   int paramX=0;
		  if(countParam > 0)
		    {
		      char *aux=NULL;
		      aux= (char *)malloc(sizeof(char)*(lenDigitY+1));
		      memcpy(aux, &params[0], lenDigitY);
		      aux[lenDigitY]='\0';
		      char * validParam=NULL;
		      paramY= strtol((const char *)&aux[0], &validParam, 10);
		      free(aux);
		      if(lenDigitX > 0)
			{
			  aux= (char *) malloc(sizeof(char)*(lenDigitX+1));
			  memcpy(aux, &params[lenDigitY], lenDigitX);
			  aux[lenDigitX]='\0';
			  validParam=NULL;
			  paramX= strtol((const char *)&aux[0], &validParam, 10);
			 free(aux);
			    
			}
		     
		      
		    }

		  switch(finalChar)
		    {
		      
		    case 'A':
		      cursor.x+=-((countParam <= 0)? 1: paramX);break;
		    case 'B':
		      cursor.y+=((countParam <= 0) ? 1:paramY);break;
		    case 'C':
		      cursor.x+= ((countParam <= 0) ? 1:paramX);break;
		    case 'D':
		      cursor.x+=-((countParam <= 0) ? 1:paramX);break;
		    case 's':oldCursor=cursor;break;
		    case 'u':cursor=oldCursor;break;
		    case 'E':
		      {
			cursor.y+= ((countParam <=0) ? 1:paramY);
		        cursor.x=0;
		      }break;
		    case 'F':
		      {
			cursor.y+=-((countParam <=0) ? 1:paramY);
		        cursor.x=0;	
		      }
		    case 'G':cursor.x=((countParam <= 0) ? 1 :paramX);break;
		    case 'd':cursor.y=((countParam <= 0) ? 1 : paramY);break;
		    case 'H':
		      {
			cursor.y= ((countParam <= 0) ? 1:paramY);
		        cursor.x= ((countParam <= 0) ? 1:paramX);
		      }break;
			
		    case 'f':
		      {
			cursor.y= ((countParam <= 0) ? 1:paramY);
		        cursor.x= ((countParam <= 0) ? 1:paramX);
			
		      }break;
		    default:
		      break;
		    }
		}
	    }
	}
    }
}

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/



BMP create_bmp(const char *name,
	       BGR * Color,
	       unsigned int widthInPixel,
	       unsigned int heightInPixel,
	       unsigned char bytePerPixel)
{
   BMP Bmp;
   BMPFILEHEADER bmpFileHeader={0};
   BMPINFOHEADER bmpInfoHeader={0};
   bmpFileHeader.bfType= BMP_TYPE;
   bmpFileHeader.bfSize= sizeof(BMPFILEHEADER);
   bmpFileHeader.bfOffBits= (sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER));
   bmpInfoHeader.biSize= sizeof(BMPINFOHEADER);
   bmpInfoHeader.biWidth= widthInPixel;
   bmpInfoHeader.biHeight= heightInPixel;
   bmpInfoHeader.biPlanes= 1;
   bmpInfoHeader.biBitCount=bytePerPixel*8; // creará una imagen de 23 bits por pixel RGB
   bmpInfoHeader.biCompression= BI_RGB;
   bmpInfoHeader.biSizeImage= widthInPixel * heightInPixel * 3;

   FILE *File = fopen(name,"wb");
   if(File)
     {
       fwrite(&bmpFileHeader,sizeof(BMPFILEHEADER),1,File);
       fwrite(&bmpInfoHeader,sizeof(BMPINFOHEADER),1,File);
       // copiar los pixeles al archivo
       double mul4 = ceil(((double)(widthInPixel * 3) / 4)) * 4;
       int pading     = (mul4 - (widthInPixel *3));
       char arrPading[3]={0x00,0x00,0x00};
       for(int row=0; row < heightInPixel; row++)
	 {
	   for(int col=0; col < widthInPixel; col++)
	     {
	       fwrite(Color, sizeof(BGR),1,File);
	     }
	   fwrite(arrPading,pading,1,File);
	 }

       fclose(File);
       File=NULL;
       load_bmp(&Bmp, name);
     }
   else
     {
       // printf("\nNo se pudo crear el archiov %s",name);
     }
  
   return Bmp;
   
}

void load_bmp(BMP *Bmp, const char *path)
{
  BMPFILEHEADER bmpFileHeader;
  BMPINFOHEADER bmpInfoHeader;
  FILE * File= fopen(path,"rb");
  if(File)
    {
      int count= fread(&bmpFileHeader, sizeof(BMPFILEHEADER),1,File);
      if(count)
	{
	  count= fread(&bmpInfoHeader,sizeof(BMPINFOHEADER),1, File);
	  if(count)
	    {
	      if(bmpFileHeader.bfType == BMP_TYPE)
		{
		  int widthBmp     = bmpInfoHeader.biWidth;
		  int heightBmp    = bmpInfoHeader.biHeight;
		 
		  unsigned char arrPading[3];
		  if(bmpInfoHeader.biBitCount > 8)
		    {
		      // sin paleta
		      BGR Color;
		      int bitPerPixel  = 32;
		      int sizeBuffer32 = 4*widthBmp*heightBmp;
		      unsigned int * bufferPixel32 =(unsigned int *) malloc(sizeBuffer32);
		      double mul4 = ceil(((double)(widthBmp * 3) / 4)) * 4;
		      int pading     = (mul4 - (widthBmp *3));

		      for(int row =0; row < heightBmp; row++)
			{
			  for(int col=0; col < widthBmp; col++ )
			    {
			      unsigned int index  = widthBmp * row + col;
			      fread(&Color,sizeof(BGR),1,File);
			      unsigned int blue    = Color.blue;
			      unsigned int green   = Color.green;
			      unsigned int red     = Color.red;
			      unsigned int color32 = ((red <<24 )|
				                     (green << 16)|
				                     (blue << 8)|
						      (0xff));

			      bufferPixel32[index]=color32;
			    }
			  fread(arrPading,pading,1,File); // saltamos el pading
			  
			}
		      flip_bmp(bufferPixel32, sizeof(int),widthBmp,heightBmp);
		      Bmp->bmBits      = (void *)bufferPixel32;
		      Bmp->palete      = NULL;
		      Bmp->bmBitsPixel = bitPerPixel;
		      Bmp->bmPitch     = widthBmp * 4;
		      Bmp->bmHeight    = heightBmp;
		      Bmp->name        = path;		      
		    }
		  else
		    {
		      // con paleta
		      // primero leer la paleta
		      // luego leer los indices que apuntan hacia los colores de la pa

		      int sizePalete;
		      if(bmpInfoHeader.biClrUsed == 0)
			sizePalete = pow((double)2,(double)bmpInfoHeader.biBitCount);
		      else
			sizePalete =bmpInfoHeader.biBitCount;
		      RGB * Palete = (RGB *)malloc(sizeof(RGB)*sizePalete);
		      fread(Palete,(sizePalete *sizeof(RGB)),1,File);
		      double mul4       = ceil(((widthBmp)/4)) * 4;
		      int pading     = (mul4 - (widthBmp));

		      unsigned char * bufferOfIndex= (unsigned char *) malloc(widthBmp*heightBmp);
		      for(int row =0; row < heightBmp; row++)
			{
			  for(int col=0; col < widthBmp; col++ )
			    {
			      unsigned int index  = widthBmp * row + col;
			      unsigned char color;
			      fread(&color,sizeof(unsigned char),1,File);
			     
			      bufferOfIndex[index]=color;
			    }
			  fread(arrPading,pading,1,File); // saltamos el pading
			  
			}
		      flip_bmp(bufferOfIndex, sizeof(char),widthBmp,heightBmp);
		      Bmp->bmBits      = (void *)bufferOfIndex;
		      Bmp->palete      = Palete;
		      Bmp->bmBitsPixel = bmpInfoHeader.biBitCount;
		      Bmp->bmPitch     = widthBmp; // puede llevar pading?
		      Bmp->bmHeight    = heightBmp;
		      Bmp->name        = path;		
		    }

		  fclose(File);
		  
		}
	      else
		{
		  printf("\n No es un archivo BMP");
		}
	      
	    }
	  else
	    {

	      printf("\n Fallo al leer la cabezera BMPFILEHEADER");
	    }
	}
      else
	{
	  printf("\n Fallo al leer la cabezera BMPFILEHEADER");
	}
    }
  else
    {
      printf("\nNo se pudo cargar el archivo");
    }

  
    
}

void flip_bmp(void * pixel, unsigned int dataType, unsigned int width, unsigned int height)
{
  unsigned int pitch= width * dataType;
  unsigned int sizeBuffer=dataType*width * height;
  unsigned char * auxBuffer = (unsigned char *) malloc(sizeBuffer);
  memcpy(auxBuffer, pixel,sizeBuffer);
  unsigned  char * dest= (unsigned char *)pixel;
  unsigned  char * source = (unsigned char *) auxBuffer;
   
  for(int row=0; row < height; row++)
    {
      int indexDest= row * pitch;
      int indexSource= (height-1 - row) * pitch;
      memcpy((dest+indexDest), (source+indexSource), pitch);
      
    }

  free(auxBuffer);
  
}

void save_bmp(BMP *Bmp,const char* nameOu)
{

  BMPFILEHEADER bmpFileHeader;
  BMPINFOHEADER bmpInfoHeader;
  FILE * File= fopen(Bmp->name,"rb");
  if(File)
    {
      int count= fread(&bmpFileHeader, sizeof(BMPFILEHEADER),1,File);
      if(count)
	{
	  count= fread(&bmpInfoHeader,sizeof(BMPINFOHEADER),1, File);
	  if(count)
	    {
	      fclose(File);
	      File = NULL;
	      File = fopen(nameOu,"wb");
	      if(!File)return; // no se pudo abri el archivo
	      fwrite(&bmpFileHeader,sizeof(BMPFILEHEADER),1,File);
	      fwrite(&bmpInfoHeader,sizeof(BMPINFOHEADER),1,File);
		
	     	  int widthBmp     = bmpInfoHeader.biWidth;
		  int heightBmp    = bmpInfoHeader.biHeight;		  		 
		  unsigned char arrPading[3];
		  if(bmpInfoHeader.biBitCount > 8)
		    {
		      // sin paleta
		      BGR Color;
		       unsigned int * bufferPixel32 = (unsigned int *)Bmp->bmBits;
		       flip_bmp(bufferPixel32, sizeof(int),widthBmp,heightBmp);
		       double mul4 = ceil(((double)(widthBmp * 3) / 4)) * 4;
		       int pading     = (mul4 - (widthBmp *3));

		      for(int row =0; row < heightBmp; row++)
			{
			  for(int col=0; col < widthBmp; col++ )
			    {
			      unsigned int index  = widthBmp * row + col;
			      unsigned char * byte=(unsigned char *)&bufferPixel32[index];
			      
			      Color.blue  = byte[1];
			      Color.green = byte[2];
			      Color.red   = byte[3];
			      fwrite(&Color,sizeof(BGR),1,File);
			    }
			  fwrite(arrPading,pading,1,File); // saltamos el pading
			}	      
		    }
		  else
		    {
		      // con paleta
		      // primero leer la paleta
		      // luego leer los indices que apuntan hacia los colores de la pa

		      int sizePalete;
		      if(bmpInfoHeader.biClrUsed == 0)
			  sizePalete = pow(2,bmpInfoHeader.biBitCount);
		      else
			sizePalete =bmpInfoHeader.biBitCount;
		      RGB * Palete = Bmp->palete;
		      fwrite(Palete,(sizePalete *sizeof(RGB)),1,File);
		      double  mul4       = ceil(((double)(widthBmp ) / 4)) * 4;
		      int  pading     = (mul4 - (widthBmp));
		      unsigned char * bufferOfIndex=(unsigned char *)Bmp->bmBits;
		      flip_bmp(bufferOfIndex, sizeof(char),widthBmp,heightBmp);
		      for(int row =0; row < heightBmp; row++)
			{
			  for(int col=0; col < widthBmp; col++ )
			    {
			      unsigned int index  = widthBmp * row + col;
			      unsigned char color= bufferOfIndex[index];
			      fwrite(&color,sizeof(unsigned char),1,File);
			     
			    }
			  fwrite(arrPading,pading,1,File); // saltamos el pading
			  
			}
		    
		    }

		  fclose(File);
	    }
	  else
	    {

	      printf("\n Fallo al leer la cabezera BMPFILEHEADER");
	    }
	}
      else
	{
	  printf("\n Fallo al leer la cabezera BMPFILEHEADER");
	}
    }
  else
    {
      printf("\nNo se pudo cargar el archivo");
    }

 
}

