/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "main.h"
#include "bsp.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES    480
#define MY_DISP_VER_RES    320

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
SemaphoreHandle_t screen_write_sem = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
	screen_write_sem = xSemaphoreCreateBinary();
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    // lv_disp_draw_buf_t *draw_buf_dsc = (lv_disp_draw_buf_t *)malloc(sizeof(lv_disp_draw_buf_t));
	static lv_disp_draw_buf_t draw_buf_dsc;
	lv_color_t *buf_3_1 = (lv_color_t *)0xD0000000;
	lv_color_t *buf_3_2 = (lv_color_t *)0xD0080000;
    // lv_color_t *buf_3_1 = (lv_color_t *)malloc(sizeof(lv_color_t) * MY_DISP_HOR_RES * MY_DISP_VER_RES);
    // lv_color_t *buf_3_2 = (lv_color_t *)malloc(sizeof(lv_color_t) * MY_DISP_HOR_RES * MY_DISP_VER_RES);
    lv_disp_draw_buf_init(&draw_buf_dsc, buf_3_1, buf_3_2,
                          MY_DISP_HOR_RES * MY_DISP_VER_RES);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc;

    /*Required for Example 3)*/
    disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
void lcd_reset(void)
{
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	osDelay(100);	
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
	osDelay(50);
}

void lcd_wr_reg(uint8_t data)
{
    uint8_t dummy;  // do a read as well so we know when to raise CS again
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(LCD_SPI, &data, &dummy, 1, 1000);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void lcd_wr_data(uint8_t data)
{
    uint8_t dummy;
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_TransmitReceive(LCD_SPI, &data, &dummy, 1, 1000);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void lcd_write_ram_prepare(void)
{
	lcd_wr_reg(0x2c);
}	 

void lcd_write_data_16bit(uint16_t data)
{	
  	// screen format 18Bit
	lcd_wr_data((data >> 8) & 0xF8); 	// RED
	lcd_wr_data((data >> 3) & 0xFC);	//GREEN
	lcd_wr_data(data << 3);				//BLUE
}

static void lcd_set_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
	lcd_wr_reg(0x2A);	
	lcd_wr_data(x_start >> 8);
	lcd_wr_data(0xFF & x_start);		
	lcd_wr_data(x_end >> 8);
	lcd_wr_data(0xFF & x_end);

	lcd_wr_reg(0x2B);	
	lcd_wr_data(y_start >> 8);
	lcd_wr_data(0xFF & y_start);		
	lcd_wr_data(y_end >> 8);
	lcd_wr_data(0xFF & y_end);

	lcd_write_ram_prepare();
}

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    lcd_reset();

	lcd_wr_reg(0XF1);
	lcd_wr_data(0x36);
	lcd_wr_data(0x04);
	lcd_wr_data(0x00);
	lcd_wr_data(0x3C);
	lcd_wr_data(0X0F);
	lcd_wr_data(0x8F);
	lcd_wr_reg(0XF2);
	lcd_wr_data(0x18);
	lcd_wr_data(0xA3);
	lcd_wr_data(0x12);
	lcd_wr_data(0x02);
	lcd_wr_data(0XB2);
	lcd_wr_data(0x12);
	lcd_wr_data(0xFF);
	lcd_wr_data(0x10);
	lcd_wr_data(0x00);
	lcd_wr_reg(0XF8);
	lcd_wr_data(0x21);
	lcd_wr_data(0x04);
	lcd_wr_reg(0XF9);
	lcd_wr_data(0x00);
	lcd_wr_data(0x08);
	lcd_wr_reg(0x36);
	lcd_wr_data(0x08);
	lcd_wr_reg(0xB4);
	lcd_wr_data(0x00);
	lcd_wr_reg(0xC1);
	lcd_wr_data(0x47); //0x41
	lcd_wr_reg(0xC5);
	lcd_wr_data(0x00);
	lcd_wr_data(0xAF); //0x91
	lcd_wr_data(0x80);
	lcd_wr_data(0x00);
	lcd_wr_reg(0xE0);
	lcd_wr_data(0x0F);
	lcd_wr_data(0x1F);
	lcd_wr_data(0x1C);
	lcd_wr_data(0x0C);
	lcd_wr_data(0x0F);
	lcd_wr_data(0x08);
	lcd_wr_data(0x48);
	lcd_wr_data(0x98);
	lcd_wr_data(0x37);
	lcd_wr_data(0x0A);
	lcd_wr_data(0x13);
	lcd_wr_data(0x04);
	lcd_wr_data(0x11);
	lcd_wr_data(0x0D);
	lcd_wr_data(0x00);
	lcd_wr_reg(0xE1);
	lcd_wr_data(0x0F);
	lcd_wr_data(0x32);
	lcd_wr_data(0x2E);
	lcd_wr_data(0x0B);
	lcd_wr_data(0x0D);
	lcd_wr_data(0x05);
	lcd_wr_data(0x47);
	lcd_wr_data(0x75);
	lcd_wr_data(0x37);
	lcd_wr_data(0x06);
	lcd_wr_data(0x10);
	lcd_wr_data(0x03);
	lcd_wr_data(0x24);
	lcd_wr_data(0x20);
	lcd_wr_data(0x00);

	// COLMOD - Pixel Format
	// Sets the interface bits per pixel
	// Upper nibble specifies RGB interface (not used here)
	// Lower nibble specifies CPU interface
	//  0x5 - 16 bits/pixel
	//  0x6 - 18 bits/pixel
	// none of this applies to SPI
	lcd_wr_reg(0x3A);
	lcd_wr_data(0x66);
	lcd_wr_reg(0x11);
	lcd_wr_reg(0x36);
	lcd_wr_data(0x28);
	osDelay(120);
	lcd_wr_reg(0x29);
	
    // set it in landscape mode
    lcd_wr_reg(0x36);
	//lcd_wr_data((1<<6)|(1<<3));//0 degree MY=0,MX=0,MV=0,ML=0,BGR=1,MH=0  Portrait
	lcd_wr_data((1<<3)|(1<<5));//90 degree MY=0,MX=1,MV=1,ML=1,BGR=1,MH=0  Landscape
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	BaseType_t woken = pdFALSE;
	xSemaphoreGiveFromISR(screen_write_sem, &woken);
	portEND_SWITCHING_ISR(woken);
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if(disp_flush_enabled)
	{
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
  		lcd_set_windows(area->x1,area->y1,area->x2,area->y2);
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);

		// two stage process, one convert from 16 bit RGB to 24 bit RGB using a manually allocated buffer in the SDRAM
		uint8_t *col_buf = (uint8_t *)0xD0100000;

		for(lv_coord_t x = area->x1;x < area->x2;x++)
		{
			for(lv_coord_t y = area->y1;y < area->y2;y++)
			{
				*col_buf ++ = (color_p->full >> 8) & 0xF8; 	// RED
				*col_buf ++ = (color_p->full >> 3) & 0xFC;	//GREEN
				*col_buf ++ = (color_p->full << 3);			//BLUE
				color_p ++;
			}
		}

		// Then send out that 24 bit colour sequence
		// but! the STM32 DMA transfers are limited at 65535 bytes! so we need to chunk this up
		uint32_t bytes_to_write = (area->y2 - area->y1) * (area->x2 - area->x1) * 3;
		uint32_t bytes_written = 0;
		while(bytes_written < bytes_to_write)
		{
			uint32_t chunk_size = bytes_to_write - bytes_written;
			if(chunk_size > 65535)
			{
				chunk_size = 65535;
			}
			HAL_StatusTypeDef err = HAL_SPI_Transmit_DMA(LCD_SPI, (uint8_t *)(0xD0100000 + bytes_written), chunk_size);

			if(err != HAL_OK)
			{
				while(1) {;}
			}
			xSemaphoreTake(screen_write_sem, portMAX_DELAY);
			bytes_written += chunk_size;
		}

		// for(int32_t i=area->y1;i<area->y2;i++)
		// {
    	// 	for(int32_t m=area->x1;m<area->x2;m++)
    	// 	{	
		// 		lcd_write_data_16bit(color_p->full);
		// 		color_p++;
		// 	}
		// }

	 	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	}

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}

