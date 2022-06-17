/*
	Raw library for ssd1306 oled-like small screens. This one
	handles	single and multiple command transactions, even with data
	from PROGMEM (mostly used for displaying imaged stored there).

	I made this for my Game of Life on an attiny85 project:
	https://github.com/theisolinearchip/gameoflife_attiny85
	https://hackaday.io/project/181421-game-of-life-on-an-attiny85

	It's the same version but without some extra methods for a small
	matrix system I made in order to display the dynamic game with
	the small SRAM available on the attiny85 (not really relevant
	for the i2c part)

	It was intended to be used with a bit-banging implementation
	for the i2c protocol I made too (same functions, different code;
	changing the base-lib doesn't matter: it also works fine! :D)
*/

#ifndef SSD1306_attiny85_c
#define SSD1306_attiny85_c

#include <avr/pgmspace.h> //ssd1306
#include "ssd1306_attiny85_constants.h"
//#include "../../AVR_SSD1306_bigchar_demo/src/OLED_DISPLAY_Hello_World_32/fonts/UbuntuMono_32.h"

#define CHAR_WIDTH 32
#define CHAR_HEIGHT 24

int cur_x, cur_y;
uint8_t page_buffer[50];

void ssd1306_send_single_command(char command) {
	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	i2c_write_byte(SSD1306_CONTROL_BYTE_ONE_COMMAND);
	i2c_write_byte(command);
	i2c_stop(); 
}

void ssd1306_set_addressing_mode(uint8_t mode)
{
    ssd1306_send_single_command(SSD1306_MEMORYMODE);
    ssd1306_send_single_command(mode);
}

void ssd1306_set_column_address(uint8_t start, uint8_t end){
    ssd1306_send_single_command(SSD1306_COLUMNADDR);
    ssd1306_send_single_command(start);
    ssd1306_send_single_command(end);
}

void ssd1306_set_page_address(uint8_t start, uint8_t end){
    ssd1306_send_single_command(SSD1306_PAGEADDR);
    ssd1306_send_single_command(start & 0x07);
    ssd1306_send_single_command(end & 0x07);
}


/* Use with uC bigger than Attiny */
/*
void ssd1306_send_buffer(const uint8_t* buffer, int length)
{
    memset(buffer, 0xff, length); 
    i2c_start();
    i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);

    for (int i = 0; i < length; ++i)
    {
        if((i>0) && (i%32==0)){
            i2c_stop(); 
            i2c_start();
            i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
        }
        i2c_write_byte(SSD1306_CONTROL_BYTE_MULTIPLE_DATA);
        i2c_write_byte(buffer[i]);
    }
    i2c_stop(); 
}
*/

void ssd1306_init() {

	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	for (int i = 0; i < 29; i++) {
		i2c_write_byte(SSD1306_CONTROL_BYTE_MULTIPLE_COMMANDS);
		i2c_write_byte(pgm_read_byte(init_options + i)); // progmem access
	}
	i2c_stop();
}

void ssd1306_send_multiple_commands(int length, char commands[]) {
	if (length <= 0) return;

	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	for (int i = 0; i < length; i++) {
		i2c_write_byte(SSD1306_CONTROL_BYTE_MULTIPLE_COMMANDS);
		i2c_write_byte(commands[i]);
	}
	i2c_stop();
}

void putcharacter(char c){
	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	i2c_write_byte(SSD1306_CONTROL_BYTE_ONE_DATA);
	c -= 32;

        if (cur_x > 120) {
            cur_y+=2;
            if (cur_y > 6) {
                cur_y = 6;
            }
                gotoxy(0, cur_y);
        } 
	if( c < 127-32 ) {
	    for (uint8_t i = 0; i < 8; i++)
	    {
		i2c_write_byte(pgm_read_byte(&ssd1306xled_font8x16[c * 16 + i]));	// print font to ram, print 6 columns
	    }
	}
	i2c_stop();

        gotoxy(cur_x, cur_y+1);
        i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	i2c_write_byte(SSD1306_CONTROL_BYTE_ONE_DATA);
            for (uint8_t i = 0; i < 8; i++)
	    {
		i2c_write_byte(pgm_read_byte(&ssd1306xled_font8x16[c * 16 + i + 8]));	// print font to ram, print 6 columns
	    }
	i2c_stop();
        gotoxy(cur_x+8, cur_y-1); 
}

void putstring(const char* s){
    while (*s) {
        putcharacter(*s++);
    }
}

void gotoxy(int x, int y){
        cur_x = x;
        cur_y = y;
	//uint8_t commandSequence[] = {0xb0+y, 0x21, x, 0x7f};
	uint8_t commandSequence[] = {0xb0+y, ((x & 0xf0) >> 4) | 0x10, (x & 0x0f) | 0x01};
	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	i2c_write_byte(SSD1306_CONTROL_BYTE_ONE_COMMAND);
	for(int i=0;i<3;i++){	
		i2c_write_byte(commandSequence[i]);
	}
	i2c_stop();
}

void plot_point (int x, int y, int mode) {
    ssd1306_set_addressing_mode(0x01);  
    ssd1306_set_column_address(x,x);
    ssd1306_set_page_address(0,7);
    i2c_start();
    i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
    i2c_write_byte(SSD1306_CONTROL_BYTE_ONE_DATA);
    for (int i=0; i<8; i++) {
    if (y > 7) i2c_write_byte(- mode);
    else if (y < 0) i2c_write_byte(0);
    else i2c_write_byte((1<<y) - mode);
    y = y - 8;
    }
    i2c_stop();
}

/// Fast 8-bit approximation of sin(x). This approximation never varies more than
/// 2% from the floating point value you'd get by doing
///
///     float s = (sin(x) * 128.0) + 128;
///
/// @param theta input angle from 0-255
/// @returns sin of theta, value between 0 and 255
static const uint8_t b_m16_interleave[8] = { 0, 49, 49, 41, 90, 27, 117, 10 };
uint8_t sin8( uint8_t theta)
{
    uint8_t offset = theta;
    if( theta & 0x40 ) {
        offset = (uint8_t)255 - offset;
    }
    offset &= 0x3F; // 0..63

    uint8_t secoffset  = offset & 0x0F; // 0..15
    if( theta & 0x40) secoffset++;

    uint8_t section = offset >> 4; // 0..3
    uint8_t s2 = section * 2;
    const uint8_t* p = b_m16_interleave;
    p += s2;
    uint8_t b   =  *p;
    p++;
    uint8_t m16 =  *p;

    uint8_t mx = (m16 * secoffset) >> 4;

    int8_t y = mx + b;
    if( theta & 0x80 ) y = -y;

    y += 128;

    return y;
}

/// @param theta input angle from 0-255
/// @returns y value (pixel height) value between 0 and 32 
uint8_t sinToPixel(uint8_t theta){
    return (33*theta)/256;    
}

/// Fast 8-bit approximation of cos(x). This approximation never varies more than
/// 2% from the floating point value you'd get by doing
///
///     float s = (cos(x) * 128.0) + 128;
///
/// @param theta input angle from 0-255
/// @returns sin of theta, value between 0 and 255
uint8_t cos8(uint8_t theta)
{
    return sin8(theta + 64);
}

/* Too slow, look into using Bresenham's algorithm */
void line(int x_i, int y_i, int x_f, int y_f, int fill){ }

/* */
void circleAnimated(uint8_t x0, uint8_t y0, uint8_t r){
    for(int i=0;i<=256;i++){
        uint8_t y = sin8(i);
        uint8_t x = cos8(i);
        uint8_t y_p = sinToPixel(y);
        uint8_t x_p = sinToPixel(x);
        
        //plot_point(i/4, y/5, 0); 
        //plot_point(((x)/(64/r))+x0, ((y)/(32/r))+y0, 0); 
        plot_point((x_p/2)+x0, y_p+y0, 1); 
    }
}



// Gaussian approximation
int e (int x, int f, int m) {
  return (f * 256) / (256 + ((x - m) * (x - m)));
}

// Demo plot
/*
void demo () {
  for (int x=0; x<128; x++) {
    int y = e(x, 40, 24) + e(x, 68, 64) + e(x, 30, 104) - 14;
    plot_point(x, y, 1);
  }
  //for (;;);
}
*/


//void drawCircle(int16_t x0, int16_t y0, int16_t r) {
/* Partially working */
void drawCircle(int x0, int y0, int16_t r) {
    int w = 2*r+1;
    int x,y;
    for(int i = 0; i<w; i++){
        for(int j = 0; j<w; j++){
            x = x0 + i-r;
            y = y0 + j-r;
            if (x*x + y*y <= r*r+1 ){
                plot_point(x, y, 0);
            }
        }
    }
    
}


void ssd1306_clear_display() {
        gotoxy(0,0);
	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	for (int i = 0; i < 1024; i++) {
		i2c_write_byte(SSD1306_CONTROL_BYTE_ONE_DATA);
		i2c_write_byte(0x00);
	}
	i2c_stop();
}

void ssd1306_send_single_data(char data) {
	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	i2c_write_byte(SSD1306_CONTROL_BYTE_ONE_DATA);
	i2c_write_byte(data);
	i2c_stop(); 
}

void ssd1306_send_multiple_data(int length, char data[]) {
	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	for (int i = 0; i < length; i++) {
		i2c_write_byte(SSD1306_CONTROL_BYTE_MULTIPLE_DATA);
		i2c_write_byte(data[i]);
	}
	i2c_stop();
}

void ssd1306_send_progmem_multiple_data(const int length, const char *data) {
	i2c_start();
	i2c_write_byte(SSD1306_I2C_WRITE_ADDRESS);
	for (int i = 0; i < length; i++) {
		i2c_write_byte(SSD1306_CONTROL_BYTE_MULTIPLE_DATA);
		i2c_write_byte(pgm_read_byte(data + i));
	}
	i2c_stop();
}

#endif
