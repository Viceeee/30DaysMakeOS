#include "bootpack.h"
#include<stdio.h>
/*   使用sprintf函数的解释
为了使用sprintf,自制操作系统中不能随便用printf函数，但是这个sprintf可以使用，因为
sprintf不是按照指定格式输出，只是将输出内容作为字符串写在内存中，
这次用的sprintf函数，是本次使用的名为GO的C编译器所附带的函数，它在制作者的精心设计下
能够不使用操作系统的任何功能，用printf的话，不管怎样都要使用操作系统的功能，而sprintf只需要对内存进行操作

*/

/* 告诉C编译器，有一个函数在别的文件里
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
//void write_mem8(int addr, int data);
	一点疑惑
	书上这么写：是函数声明却不用{}，而用;，表示函数在别的文件中，自己去找
	但是我觉得不是，这个是Makefile里面所规定的文件吧，虽然它上面的这些个是在nas文件里的函数，是通过makefi
	在这么多文件中搞了一个链接吧？



//就算写在同一个源文件里，如果想在定义前使用，还是必须事先声明一下
void init_palette(void);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void set_palette(int start, int end, unsigned char *rgb);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, 
	int pysize, int px0, int py0, char *buf, int bxsize);
	为什么不在这里添加gdb_init? 
	void init_gdtidt(void);
	void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
	void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

 */

/*	指针的解释：
	指针方法一：
	for(i = 0xa0000; i <= 0xaffff; i++)
	{
		//write_mem8(i, 15); // MOV BYTE [i],15 
		//write_mem8(i,i & 0x0f); // MOV BYTE [i],15 
		p = (char *)i ;//代入地址
		*p = i & 0x0f;
		//可以代替write_mem8(i,i & 0x0f);
	}
	方法二：
	p = (char *) 0xa0000;//给地址变量赋值
	
	for(i = 0; i <= 0xffff; i++)
	{
		*(p + i) = i & 0x0f;
	}
	
*/

/* 定义颜色
#define COL8_000000		0	//black
#define COL8_FF0000		1	//bright red
#define COL8_00FF00		2	//bright grenn
#define COL8_FFFF00		3	//bright yellow
#define COL8_0000FF		4	//bright blue
#define COL8_FF00FF		5	//bright purpore
#define COL8_00FFFF		6	//bright light blue
#define COL8_FFFFFF		7	//white
#define COL8_C6C6C6		8	//bright grey
#define COL8_840000		9	//	dark red
#define COL8_008400		10	//dark green
#define COL8_848400		11	//dark yellow 
#define COL8_000084		12	//dark green grass
#define COL8_840084		13	//dark purpore
#define COL8_008484		14	//light dark blue
#define COL8_848484		15	//dark grey */

/*	结构体的说明
# 普通的结构体使用方法
结构体的好处是可以像下面这样讲各种东西都一股脑的传过来
func(abc);
如果没有结构体，就只能讲各个参数一个一个的传过来
func(scrnx, scrny, vram, ....);
所以很多时候都会将有某种意义的数据都归纳到一个结构体里面，这样就方便多了
但是如果归纳方法错了，就很多麻烦
结构体内部的变量是要这样写的：在结构体变量名的后面加一个点（.），然后在写变量名，这是规则


struct BOOTINFO abc:
abc.scrnx = 320;
abc.scrny = 200;
abc.vram  = 0xa0000;


下一步是使用指针。5.2节中使用方法。声明方法如下：
变量类型名 *指针变量名;（char *p;)
而本次的变量类型是 struct BOOTINFO, 变量名是binfo，所以写成如下形式：
struct BOOTINFO *binfo;

因为binfo表示指针变量。地址用4个字节来表示，所以binfohi4字节变量。
因为是指针变量，所以应该首先给指针赋值，否则就不知道要往哪里读写了，
可以写成这样：（本来是 binfo = 0x0ff0; 但因为出warning，所以加了类型转换
binfo = (struct BOOTINFO *)0x0ff0;

设定了指针地址后，这12个字节的结构体用起来就OK了，这样我们可以不再
直接使用内存地址，而是使用*binfo来表示这个内存地址上12字节的结构体。这与"char *p"
中的*p表示p地址的1字节是同样道理。
要想表示结构体变量ABC中的scrnx时，就用abc.scrnx,与此类似，这里用(*binfo).scrnx表示 


*/
/* 转移到了bootpack。h
	struct BOOTINFO
{	
	char cyls, leds, vmode, reserve;	//一字节的cyls，leds，vmode，reserve变量
	short scrnx, scrny;					//
	char *vram;
}; */

/* 	移到了dsctbl.c中
	struct SEGMENT_DESCRIPTOR
{
		SEGMENT_DESCRIPTOR结构体
		SEGMENT_DESCRIPTOR中存放GDT的8字节内容，
		是以CPU的资料为基础，写成结构体的形式
	
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR
{	
	GATE_DESCRIPTOR结构体
		GATE_DESCRIPTOR也是以CPU的资料为基础，写成了结构体的形式


	
	short offset_low, selector;
	
	char dw_count, access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
 */




void HariMain (void) 
{
	/*	前面用到的变量声明
		int i;//i是一个32位整数
		char *p;//变量p，用于BYTE2型地址,是BYTE[...]用的地址
		char *vram;//变量vram，用于BYTE2型地址,是BYTE[...]用的地址
		int xsize, ysize;
	
		short *binfo_scrnx, *binfo_scrny;
		int  *binfo_vram;//+这里已经在struct里面声明过了
	//struct BOOTINFO *binfo;//在这里声明 *binfo是struct BOOTINFO的结构体变量*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;//这个是把上面一句和下面的binfo = 0x0ff0合并了
	//extern char hankaku[4096];
	char  mcursor[256], s[40];
	int mx, my;
	
	
	/*	之前显示A字符的16位
		static char font_A[16] = {
			0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
			0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00, 
		};
	*/
	/*	箭头记号
		使用箭头记号，可以将"xsize = (*binfo).scrnx;" 写成"xsize = binfo->scrnx"
		binfo = (struct BOOTINFO *) 0X0ff0;
		vram = (char *) *binfo_vram;//将地址赋值进vram
		xsize = (*binfo).scrnx;
		ysize = (*binfo).scrny;
	*/
	init_gdtidt();
	init_palette();//设定调色板
	init_screen(/*vram = */binfo->vram, /*xsize =*/ binfo->scrnx, /*ysize = */binfo->scrny);
	mx = (binfo->scrnx - 16) / 2;//计算屏幕中心的坐标
	my = (binfo->scrny - 28 - 16) /2;
	/*	之前在显示屏幕上打的ABC 123 xyz和haribote os和sprintf函数的显示
		//putfonts8_asc(binfo->vram, binfo->scrnx,  8,   8,COL8_FFFFFF, "ABC 123 xyz");
		//putfonts8_asc(binfo->vram, binfo->scrnx,  31, 31,COL8_C6C6C6, "Haribote OS");
		//putfonts8_asc(binfo->vram, binfo->scrnx,  31, 54,COL8_FFFFFF, "Haribote os");
		//sprintf( s , "scrnx = %d", binfo-> scrnx);
		//putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF,s);
	*/
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "( %d ,%d )", mx, my);//显示在屏幕上的字符
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);//字符适配
	
	
	/* 指针赋值
		p = (char *) 0xa0000;//将地址赋值进p
	*/
	/*	screen函数的矩形
		原来screen函数里面的几个矩形是放在这里的，后面创建了一个screen函数就把它放到screen里了
		for(i = 0; i <= 0xffff; i++)
		{
			p[i] = i & 0x0f;
		}*/
	
	
	
	for(;;)
	{
		io_hlt();						/*执行naskfunc里的_io_hlt  */
	}
	
	return;

}
/* 移到了graphic.c
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
	//准备鼠标指针(16*16)
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
/* 	{
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
 */ 
/* 	移到了dsctbl.c
	void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
		变量gdt的含义
		变量gdt被赋值0x00270000,就是说要将0x270000~0x27ffff设为GDT。
		这个地址是随意选的，没有特殊含义。
	
	
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;
		idt的作用
		idt被设定成了0x26f800~0x26ffff,这个内存中间会存放bootpack.h
		后面的asmhead.nas 文件会处理这些信息。
	
	
	int i;
	GDT的初始化
	for (i = 0; i < 8192; i++)
	{
		
		set_segmdesc(gdt + i, 0, 0, 0);
			对循环和函数变量的解释
			gdt是0x2700000,i从0开始，每次加1，知道8191，这样一来，gdt+i最大也只能是0x271fff。
			但是实际上是：C语言中进行指针的加法运算时，内部还隐含着乘法运算。变量gdt已经声明
			为指针，指向SEGMENT_DESCRIPTOR这样一个8字节的结构体，所以往gdt里加一，结果是地址
			加了8。
			因此这个for循环语句就完成了对所有8192个段的设定，将他们的上限（limit，指段的字节数-1）
			基址（base）、访问权限都设为0。
			
		
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
		对上面两个函数的解释：
	
		以上语句是对段号为1和2的两个段进行的设定。段号为1和2的两个段进行的设定。
		段号为1的段，上限值为0xffffffff即大小正好是4GB，它表示的是CPU所能管理的全部内存本身。
		段号为2的段，大小是512KB，地址是0x280000。这正好是为了bootpack.hrb而准备的，
		用这个段，就可以执行bootpack.hrb. 因为bootpack.hrb是以ORG 0 为前提翻译成的机器语言
		
	
	
	load_gdtr(0xffff, 0x00270000);//这是因为依照常规，C语言中不能给GDTR赋值，所以要借助汇编的力量
	IDT的初始化
	for (i = 0; i < 256; i++)
	{
		set_gatedesc(idt + i, 0, 0, 0 );
	}
	load_idtr(0x7ff, 0x0026f800);
	
	return ;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	if(limit > 0xffffff)
	{
		ar |= 0x8000;//G_bit = 1 
			what the fuck about |=
			他是ar = ar|0x8000的省略表现形式，同理还有下面的limit /= 0x1000;
			"|"是or运算符
		 
		limit /= 0x1000;
	}
	sd->limit_low	 = limit & 0xffff;
	sd->base_low	 = base & 0xffff;
	sd->base_mid	 = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high	 = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high	 = (base >> 24) & 0xff;
	
		关于>>的解释    ？？？
		>>是右移位运算符，很复杂，没搞懂
	
	
	
	return ;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low	 = offset &0xffff;
	gd->selector	 = selector;
	gd->dw_count	 = (ar >> 8) & 0xff;
	gd->access_right =  ar & 0xff;
	gd->offset_high	 = (offset >>16) & 0xffff;
	return ;
}
 */















