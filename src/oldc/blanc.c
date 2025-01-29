int y;
  for (y = 0; y < width; y++)
    {
      if(y  >= height)
       {
	  if (y == width / 3)
	    {
	      wchar_t welcome[80];
	     
	      int kilov=5;
	      int welcomelen = swprintf(welcome,80,L"TERMILA EMULADOR - VT100  %d", kilov);
	      printf(welcome);
	      if(welcomelen > width) welcomelen = width;
	      int padding = (width - welcomelen) / 2;
	      if (padding)
		{
		   appendBuffer(L"~",wcslen(L"~"));
		  padding--;
		}
	      while (padding--)
		{
		    appendBuffer(L" ",wcslen(L" "));
		}
	       appendBuffer(welcome, welcomelen);
	    }
	  else
	    {
	       appendBuffer(L"~",wcslen(L"~"));
	    }
       }
      else
	{
	  
	}
      appendBuffer(L"\x1b[K",0);
      if (y < width-1)
	{
	  appendBuffer(L"\n\r",wcslen(L"\n\r"));
	}
    }
