#include <stdio.h>
#include <stdint.h>

void line(int x_i, int y_i, int x_f, int y_f){
    int height = 64;
    int y_d;
    int x_diff = x_f-x_i;
    int y_diff = y_f-y_i;
    int n; 
    for(int i=0;i<x_diff;i++){
        n = (((y_diff*i))/x_diff);
        if(n>0){
            y_d = height - n; 
        } else {
            y_d = abs(n); 
        }
        //plot_point(i, y_d, 0);
        printf("%d %d\n", x_i+i, y_d); 
    }
}

static const uint8_t b_m16_interleave[8] = { 0, 49, 49, 41, 90, 27, 117, 10 };
uint8_t sin8_C( uint8_t theta)
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

int main(){
    //int new = (0b00000001 & 0b11111111);
    //printf("%d\n", new); 

    for(int i = 0; i<=256; i++){
        printf("%d %d\n", i, sin8_C(i)); 
    }

        //line(0,0,64,64);
        //line(64,64,128,0);
}
