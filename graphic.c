#include "bootpack.h"

void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = 
	{
		0x00, 0x00, 0x00,//0:black
		0xff, 0x00, 0x00,//1:bright red
		0x00, 0xff, 0x00,//2:bright green
		0xff, 0xff, 0x00,//3:bright yellow
		0x00, 0x00, 0xff,//4:bright blue
		0xff, 0x00, 0xff,//5:bright purpure
		0x00, 0xff, 0xff,//6:bright light  blue
		0xff, 0xff, 0xff,//7:white
		0xc6, 0xc6, 0xc6,//8:bright grey
		0x84, 0x00, 0x00,//9:dark red
		0x00, 0x84, 0x00,//10:dark green
		0x84, 0x84, 0x00,//11:dark yellow
		0x00, 0x00, 0x84,//12:dark green grass
		0x84, 0x00, 0x84,//13:dark purpure
		0x00, 0x84, 0x84,//14:light dark blue
		0x84, 0x84, 0x84 //15:dark grey
	};
	set_palette(0, 15, table_rgb);
	return;
	//C语言中的static char语句只能用于数据，相当于汇编中的DB指令
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i,eflags;
	eflags = io_load_eflags();									//记录中断许可标志的值
	io_cli();												  //将中断许可标志置为0，禁止中断
	io_out8(0x03c8,start);
	for (i = start; i <= end; i++)						   		//往指定装置里面传送数据的函数
	{
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);										//复原中断许可标志
	return ;
}
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for(y = y0; y <= y1; y++)
	{
		for(x = x0;x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_screen(char *vram, int x, int y)
{
	/**/
	boxfill8(vram, x, COL8_008484,  0,	0, 	  x-1, y-29);
	boxfill8(vram, x, COL8_C6C6C6,  0,	y-28, x-1, y-28);
	boxfill8(vram, x, COL8_FFFFFF, 	0, 	y-27, x-1, y-27);
	boxfill8(vram, x, COL8_C6C6C6, 	0, 	y-26, x-1, y- 1);
	
	boxfill8(vram, x, COL8_FFFFFF, 	3, 	y-24, 59,  y-24);
  //boxfill8(vram, x, COL8_FFFFFF, 	3, 	y-24,  2,  y-27);//为什么这一行没了？
	boxfill8(vram, x, COL8_FFFFFF, 	2, 	y-24,  2,  y- 4);
	boxfill8(vram, x, COL8_848484, 	3, 	y- 4, 59,  y- 4);
	boxfill8(vram, x, COL8_848484, 	59, y-23, 59,  y- 5);
	boxfill8(vram, x, COL8_000000, 	2, 	y- 3, 59,  y- 3);
	boxfill8(vram, x, COL8_000000, 	60, y-24, 60,  y- 3);
	
	boxfill8(vram, x, COL8_848484,  x-47, y-24, x- 4, y-24);
	boxfill8(vram, x, COL8_848484, 	x-47, y-23, x-47, y- 4);
	boxfill8(vram, x, COL8_FFFFFF, 	x-47, y- 3, x- 4, y- 3);
	boxfill8(vram, x, COL8_FFFFFF, 	x- 3, y-24, x- 3, y- 3);
	return ;
}



void putfont8(char *vram, int  xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d;//data
	for(i = 0; i < 16; i++)
	{
		p = vram +(y + i) *xsize +x;
		d = font[i];
		if((d &0x80) != 0) 
			{ p[0] = c;}
		if((d &0x40) != 0) 
			{ p[1] = c;}
		if((d &0x20) != 0)
			{ p[2] = c;}
		if((d &0x10) != 0)
			{ p[3] = c;}
		if((d &0x08) != 0) 
			{ p[4] = c;}
		if((d &0x04) != 0)
			{ p[5] = c;}
		if((d &0x02) != 0)
			{ p[6] = c;}
		if((d &0x01) != 0)
			{ p[7] = c;}
	}
	return;
}

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	extern char hankaku[4096];
	for( ; *s != 0x00; s++)
	{
		putfont8(vram,xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}
	

void init_mouse_cursor8(char *mouse, char bc)
{
	/* 准备鼠标指针(16*16)*/
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;
	
	for(y = 0; y < 16; y++)
	{
		for(x = 0; x < 16; x++)
		{
			if(cursor[y][x] =='*')
			{
				mouse[y * 16 + x] = COL8_000000;
			}
			if(cursor[y][x] =='O')
			{
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if(cursor[y][x] == '.')
			{
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return ;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
				  int pysize, int px0, int py0, char *buf, int bxsize)
/*	函数变量解释：
	里面的变量很多，其中VRam和vxsize是关于VRam的信息，他们的值分别是0xa0000和320 
	pxsize和pysize是想要现实的 图形的大小，鼠标指针的大小是16*16，所以这两个值都是16
	px0和py0指定图形在画面上的显示位置。最后的buf和bxsize分别指定图形的存放地址和每一行含有的像素数。
	bxsize和pxsize大体相同，但也有时候想放入不同的值，所以还是要分别指定这两个值
*/
	{
		int x, y;
		for (y = 0; y < pysize; y++)
		{
			for (x = 0; x < pxsize; x++)
			{
				vram[(py0 +y) * vxsize +(px0 + x)] = buf[y * bxsize + x];
			}
		}
		return ;
	}
