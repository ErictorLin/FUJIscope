/*********************************************************************************************************
*
* File                : LCD.c
* Hardware Environment:
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  :
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LCD.h"

#include "AsciiLib.h"
#include "delay.h"

/* Private define ------------------------------------------------------------*/

//#define LCD_REG              (*((volatile unsigned short *) 0x6F000000)) /* RS = 0 */
//#define LCD_RAM              (*((volatile unsigned short *) 0x6F010000)) /* RS = 1 */

/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  LCD    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */
#define  HX8347D    10 /* 0x0047 */
#define  HX8347A    11 /* 0x0047 */
#define  LGDP4535   12 /* 0x4535 */
#define  SSD2119    13 /* 3.5 LCD 0x9919 */

/* Private function prototypes -----------------------------------------------*/


/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Delay(unsigned char i)
{
	while(i--);
}


/*******************************************************************************
* Function Name  : LCD_Configuration
* Description    : Configure the LCD Control pins and FSMC Parallel interface
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOC and GPIOE clocks */
	//don't need these code , will be written in gpio.c "GPIO_Init"
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);


	    /* configure DATA PORT  pins*/
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(ILI9325_DATA_PORT, &GPIO_InitStructure);

    /* configure CONTROL PORT pins */
	GPIO_InitStructure.Pin = ILI9325_RESET_PIN | ILI9325_nWR_PIN | ILI9325_nRD_PIN | ILI9325_CS_PIN | ILI9325_RS_PIN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP ;
	HAL_GPIO_Init(ILI9325_CTRL_PORT, &GPIO_InitStructure);

    /* configure BACKLIGHT pin */
	GPIO_InitStructure.Pin = ILI9325_BACKLIGHT_PIN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP ;
	HAL_GPIO_Init(ILI9325_BACKLIGHT_PORT, &GPIO_InitStructure);
}


/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    :
* Input          : - index:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void LCD_WriteIndex(uint16_t index)
{
	Clr_Rs;
	Set_nRd;
	LCD_Delay(0);
	GPIOE->ODR = index;	 /* GPIO_Write(GPIOE,index); */
	LCD_Delay(0);
	Clr_nWr;
	Set_nWr;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    :
* Input          : - index:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void LCD_WriteData(uint16_t data)
{
	Set_Rs;
	LCD_Delay(0);
	GPIOE->ODR = data;	 /* GPIO_Write(GPIOE,data); */
	LCD_Delay(0);
	Clr_nWr;
	Set_nWr;
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    :
* Input          : None
* Output         : None
* Return         :
* Attention		 : None
*******************************************************************************/
__inline uint16_t LCD_ReadData(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t value;

	Set_Rs;
	Set_nWr;
	Clr_nRd;

	//Set ALL data pin to input
    GPIO_InitStructure.Pin = GPIO_PIN_All;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(ILI9325_DATA_PORT, &GPIO_InitStructure);

	//value = GPIO_ReadInputData(ILI9325_DATA_PORT);
    //value = GPIO_ReadInputData(ILI9325_DATA_PORT);

 	value = ((uint16_t)ILI9325_DATA_PORT->IDR);
 	value = ((uint16_t)ILI9325_DATA_PORT->IDR);

	//Set ALL data pin to onput

    GPIO_InitStructure.Pin = GPIO_PIN_All;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(ILI9325_DATA_PORT, &GPIO_InitStructure);

    Set_nRd;

    return value;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */
	Clr_Cs;
	LCD_WriteIndex(LCD_Reg);
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);
	Set_Cs;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention		 : None
*******************************************************************************/
__inline uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t LCD_RAM;

	/* Write 16-bit Index (then Read Reg) */
	Clr_Cs;
	LCD_WriteIndex(LCD_Reg);
	/* Read 16-bit Reg */
	LCD_RAM = LCD_ReadData();
	Set_Cs;
	return LCD_RAM;
}

static void LCD_SetCursor( uint16_t Xpos, uint16_t Ypos )
{


  	uint16_t temp;

	#if (DISP_ORIENTATION == 0)

	#elif (DISP_ORIENTATION == 90)
		temp = Xpos;
		Xpos = Ypos;
		Ypos = MAX_X - 1 - temp;
	#elif (DISP_ORIENTATION == 180)
		Xpos = MAX_X - 1 - Xpos;
		Ypos = MAX_Y - 1 - Ypos;
	#elif (DISP_ORIENTATION == 270)
		temp = Ypos;
		Ypos = Xpos;
		Xpos = MAX_Y - 1 - temp;
	#endif


	if (LCD_Code == LCD)
	{

			 LCD_WriteReg(0x004e, Xpos);
      		 LCD_WriteReg(0x004f, Ypos);
	}
	else //9325
	{

			LCD_WriteReg(0x0020, Xpos);
			LCD_WriteReg(0x0021, Ypos);
			//LCD_WriteIndex(0x0022);
	}


}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay Time
* Input          : - cnt: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void delay_ms(uint16_t ms)
{
	uint16_t i,j;
	for( i = 0; i < ms; i++ )
	{
		for( j = 0; j < 1141; j++ );
	}
}

/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : SSD1963 Resets
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Initializtion(void)
{

	LCD_Configuration();

  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

	Clr_Reset;

	delay_ms(100);

  	Set_Reset;
  	Set_Backlight;


	/* Set MN(multipliers) of PLL, VCO = crystal freq * (N+1) */
	/* PLL freq = VCO/M with 250MHz < VCO < 800MHz */
	/* The max PLL freq is around 120MHz. To obtain 120MHz as the PLL freq */


		LCD_Code = ILI9325;
		LCD_WriteReg(0x00e7,0x0010);
		LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */

		LCD_WriteReg(0x0001,(0<<10)|(1<<8));

		LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
	#if (DISP_ORIENTATION == 0)
		LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3));
	#elif (DISP_ORIENTATION == 90)
		LCD_WriteReg(0x0003,(1<<12)|(0<<5)|(1<<4)|(1<<3));
	#elif (DISP_ORIENTATION == 180)
		LCD_WriteReg(0x0003,(1<<12)|(0<<5)|(0<<4)|(0<<3));
	#elif (DISP_ORIENTATION == 270)
    LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(0<<4)|(1<<3));
	#endif
		LCD_WriteReg(0x0004,0x0000);
		LCD_WriteReg(0x0008,0x0207);
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000a,0x0000); 	/* display setting */
		LCD_WriteReg(0x000c,0x0001);	/* display setting */
		LCD_WriteReg(0x000d,0x0000);
		LCD_WriteReg(0x000f,0x0000);
		/* Power On sequence */
		LCD_WriteReg(0x0010,0x0000);
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);
		LCD_WriteReg(0x0013,0x0000);
		delay_ms(50);  /* delay 50 ms */
		LCD_WriteReg(0x0010,0x1590);
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50);  /* delay 50 ms */
		LCD_WriteReg(0x0012,0x009c);
		delay_ms(50);  /* delay 50 ms */
		LCD_WriteReg(0x0013,0x1900);
		LCD_WriteReg(0x0029,0x0023);
		LCD_WriteReg(0x002b,0x000e);
		delay_ms(50);  /* delay 50 ms */
		delay_ms(50);  /* delay 50 ms */
		LCD_WriteReg(0x0030,0x0007);
		LCD_WriteReg(0x0031,0x0707);
		LCD_WriteReg(0x0032,0x0006);
		LCD_WriteReg(0x0035,0x0704);
		LCD_WriteReg(0x0036,0x1f04);
		LCD_WriteReg(0x0037,0x0004);
		LCD_WriteReg(0x0038,0x0000);
		LCD_WriteReg(0x0039,0x0706);
		LCD_WriteReg(0x003c,0x0701);
		LCD_WriteReg(0x003d,0x000f);
		delay_ms(50);  /* delay 50 ms */
		LCD_WriteReg(0x0050,0x0000);
		LCD_WriteReg(0x0051,0x00ef);
		LCD_WriteReg(0x0052,0x0000);
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0xa700);
 		LCD_WriteReg(0x0061,0x0001);
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
		LCD_WriteReg(0x0090,0x0010);
		LCD_WriteReg(0x0092,0x0600);
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0110);
		LCD_WriteReg(0x0097,0x0000);
		LCD_WriteReg(0x0098,0x0000);
		/* display on sequence */
		LCD_WriteReg(0x0007,0x0133);


        delay(50);

	   LCD_Clear(Black);
    	delay(500);
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    :
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index=0;

	LCD_SetCursor(0,0);
	Clr_Cs;
	LCD_WriteIndex(0x0022);
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_WriteData(Color);
	}
	Set_Cs;
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB convert to BBBBBGGGGGGRRRRR
* Input          : RGB color
* Output         : None
* Return         : RGB color
* Attention		 :
*******************************************************************************/
static uint16_t LCD_BGR2RGB(uint16_t color)
{
	uint16_t  r, g, b, rgb;

	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;

	rgb =  (b<<11) + (g<<5) + (r<<0);

	return( rgb );
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : Get color of the point
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate
* Output         : None
* Return         : Screen Color
* Attention		 : None
*******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
	uint16_t dummy;

	LCD_SetCursor(Xpos,Ypos);
	Clr_Cs;
	LCD_WriteIndex(0x0022);

	switch( LCD_Code )
	{
		case ST7781:
		case LGDP4531:
		case LGDP4535:
		case LCD:
		case SSD1298:
      dummy = LCD_ReadData();
      dummy = LCD_ReadData();
      Set_Cs;
 		  return  dummy;
    case HX8347A:
	  case HX8347D:
    {
      uint8_t red,green,blue;
      red = LCD_ReadData()>>3;
      green = LCD_ReadData()>>3;
      blue = LCD_ReadData()>>2;
      dummy = ( green << 11 ) | (blue << 5 ) | red;
		}
      Set_Cs;
      return  dummy;
    default:	/* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
      dummy = LCD_ReadData();
      dummy = LCD_ReadData();
      Set_Cs;
      return  LCD_BGR2RGB( dummy );
	}
}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    :
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}



/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x0:
*                  - y0:
*       				   - x1:
*       		       - y1:
*                  - color:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
  short dx,dy;
  short temp;

  if( x0 > x1 )
  {
    temp = x1;
    x1 = x0;
    x0 = temp;
  }
  if( y0 > y1 )
  {
    temp = y1;
    y1 = y0;
    y0 = temp;
  }

  dx = x1-x0;
  dy = y1-y0;

  if( dx == 0 )
  {
    do
    {
      LCD_SetPoint(x0, y0, color);
      y0++;
    }
    while( y1 >= y0 );
    return;
  }
  if( dy == 0 )
  {
    do
    {
      LCD_SetPoint(x0, y0, color);
      x0++;
    }
    while( x1 >= x0 );
		return;
  }

	/* Bresenham's line algorithm  */
  if( dx > dy )
  {
    temp = 2 * dy - dx;
    while( x0 != x1 )
    {
	    LCD_SetPoint(x0,y0,color);
	    x0++;
	    if( temp > 0 )
	    {
	      y0++;
	      temp += 2 * dy - 2 * dx;
	 	  }
      else
      {
			  temp += 2 * dy;
			}
    }
    LCD_SetPoint(x0,y0,color);
  }
  else
  {
    temp = 2 * dx - dy;
    while( y0 != y1 )
    {
	 	  LCD_SetPoint(x0,y0,color);
      y0++;
      if( temp > 0 )
      {
        x0++;
        temp+=2*dy-2*dx;
      }
      else
			{
        temp += 2 * dy;
			}
    }
    LCD_SetPoint(x0,y0,color);
	}
}
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    LCD_SetPoint(x0, y0+r, color);
    LCD_SetPoint(x0, y0-r, color);
    LCD_SetPoint(x0+r, y0, color);
    LCD_SetPoint(x0-r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        LCD_SetPoint(x0 + x, y0 + y, color);
        LCD_SetPoint(x0 - x, y0 + y, color);
        LCD_SetPoint(x0 + x, y0 - y, color);
        LCD_SetPoint(x0 - x, y0 - y, color);

        LCD_SetPoint(x0 + y, y0 + x, color);
        LCD_SetPoint(x0 - y, y0 + x, color);
        LCD_SetPoint(x0 + y, y0 - x, color);
        LCD_SetPoint(x0 - y, y0 - x, color);
    }
}

void LCD_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    int16_t i;

    for (i=y0-r; i<=y0+r; i++)
        LCD_SetPoint(x0, i, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        for (i=y0-y; i<=y0+y; i++) {
            LCD_SetPoint(x0+x, i, color);
            LCD_SetPoint(x0-x, i, color);
        }
        for (i=y0-x; i<=y0+x; i++) {
            LCD_SetPoint(x0+y, i, color);
            LCD_SetPoint(x0-y, i, color);
        }
    }
}

void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	if (x > MAX_X)
		x = MAX_X;
	if (y > MAX_Y)
		y = MAX_Y;

	if ((x+w) > MAX_X)
		w = MAX_X - x;

	if ((y+h) > MAX_Y)
		h = MAX_Y - y;

	LCD_DrawLine(x, y, x, y+h, color);
	LCD_DrawLine(x, y, x+w, y, color);
	LCD_DrawLine(x+w, y+h, x, y+h, color);
	LCD_DrawLine(x+w, y+h, x+w, y, color);
}

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	uint16_t x_index, y_index;

	if (x > MAX_X)
		x = MAX_X;
	if (y > MAX_Y)
		y = MAX_Y;

	if ((x+w) > MAX_X)
		w = MAX_X - x;

	if ((y+h) > MAX_Y)
		h = MAX_Y - y;

	for(x_index = x; x_index < x+w; x_index++)
	{
		for(y_index = y; y_index < y+h; y_index++)
		{
			LCD_SetPoint(x_index, y_index, color);
		}
	}

}

void LCD_DrawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	if (x0 > MAX_X)
		x0 = MAX_X;
	if (y0 > MAX_Y)
		y0 = MAX_Y;
	if (x1 > MAX_X)
		x1 = MAX_X;
	if (y1 > MAX_Y)
		y1 = MAX_Y;
	if (x2 > MAX_X)
		x2 = MAX_X;
	if (y2 > MAX_Y)
		y2 = MAX_Y;

	LCD_DrawLine(x0, y0, x1, y1, color);
	LCD_DrawLine(x0, y0, x2, y2, color);
	LCD_DrawLine(x2, y2, x1, y1, color);
}
void LCD_DrawCross(uint16_t Xpos, uint16_t Ypos, uint16_t in_color, uint16_t out_color)
{
  LCD_DrawLine(Xpos-15,Ypos,Xpos-2,Ypos,in_color);
  LCD_DrawLine(Xpos+2,Ypos,Xpos+15,Ypos,in_color);
  LCD_DrawLine(Xpos,Ypos-15,Xpos,Ypos-2,in_color);
  LCD_DrawLine(Xpos,Ypos+2,Xpos,Ypos+15,in_color);

  LCD_DrawLine(Xpos-15,Ypos+15,Xpos-7,Ypos+15,out_color);
  LCD_DrawLine(Xpos-15,Ypos+7,Xpos-15,Ypos+15,out_color);

  LCD_DrawLine(Xpos-15,Ypos-15,Xpos-7,Ypos-15,out_color);
  LCD_DrawLine(Xpos-15,Ypos-7,Xpos-15,Ypos-15,out_color);

  LCD_DrawLine(Xpos+7,Ypos+15,Xpos+15,Ypos+15,out_color);
  LCD_DrawLine(Xpos+15,Ypos+7,Xpos+15,Ypos+15,out_color);

  LCD_DrawLine(Xpos+7,Ypos-15,Xpos+15,Ypos-15,out_color);
  LCD_DrawLine(Xpos+15,Ypos-15,Xpos+15,Ypos-7,out_color);
}
/******************************************************************************
* Function Name  : PutChar
* Description    :
* Input          : - Xpos:
*                  - Ypos:
*				           - ASCI:
*				           - charColor:
*				           - bkColor:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/

void putChar(uint16_t Xpos, uint16_t Ypos, uint8_t ASCII, uint16_t charColor, uint16_t bkColor)
{
	putCharFont(Xpos, Ypos, ASCII, charColor, bkColor, SYSTEM_8x16);
}
void putCharFont( uint16_t Xpos, uint16_t Ypos, uint8_t ASCII, uint16_t charColor, uint16_t bkColor, uint16_t FONTx)
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    uint8_t len_x, len_y;

    switch (FONTx)
    {
    	case FONT6x8:
    		len_x = 6;
    		len_y = 8;
    		break;
    	case FONT8x8:
    		len_x = 8;
    		len_y = 8;
    		break;
    	case MS_GOTHIC_8x16:
    	case SYSTEM_8x16:
    	default:
    		len_x = 8;
    		len_y = 16;
    		break;
    }

    GetASCIICode(buffer, ASCII, FONTx);
    for( i=0; i<len_y; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<len_x; j++ )
        {
            if( (((tmp_char>>7)-j) & 0x01) == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );
            }
        }
    }
}


void cleanPutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCII, uint16_t charColor)
{
	 cleanPutCharFont(Xpos, Ypos, ASCII, charColor, MS_GOTHIC_8x16);
}

void cleanPutCharFont( uint16_t Xpos, uint16_t Ypos, uint8_t ASCII, uint16_t charColor, uint16_t FONTx)
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    uint8_t len_x, len_y;

    switch (FONTx)
    {
    	case FONT6x8:
    		len_x = 6;
    		len_y = 8;
    		break;
    	case FONT8x8:
    		len_x = 8;
    		len_y = 8;
    		break;
    	case MS_GOTHIC_8x16:
    	case SYSTEM_8x16:
    	default:
    		len_x = 8;
    		len_y = 16;
    		break;
    }

    GetASCIICode(buffer, ASCII, FONTx);
    for( i=0; i<len_y; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<len_x; j++ )
        {
            if(( (tmp_char >>( 7 - j)) & 0x01 )== 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );
            }

        }
    }
}

/******************************************************************************
* Function Name  : LCD_Text
* Description    :
* Input          : - Xpos:
*                  - Ypos:
*				           - str:
*				           - charColor:
*				           - bkColor:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*
Available font type:
    MS_GOTHIC_8x16
    SYSTEM_8x16
    FONT6x8
    FONT8x8
*/
void LCD_print(uint16_t Xpos, uint16_t Ypos, char *str)
{
	LCD_printColorFont(Xpos, Ypos, str, Default_Color , MS_GOTHIC_8x16);
}
void LCD_printColor(uint16_t Xpos, uint16_t Ypos, char *str, uint16_t Color)
{
	LCD_printColorFont(Xpos, Ypos, str, Default_Color , MS_GOTHIC_8x16);

}
void LCD_printColorFont(uint16_t Xpos, uint16_t Ypos, char *str, uint16_t Color, uint16_t FONTx)
{
   char TempChar;

    do
    {
        TempChar = *str++;
        cleanPutCharFont( Xpos, Ypos, TempChar, Color, FONTx);
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        }
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }
        else
        {
            Xpos = 0;
            Ypos = 0;
        }
    }
    while ( *str != 0 );
}
