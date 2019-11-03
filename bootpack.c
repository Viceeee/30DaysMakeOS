#include "bootpack.h"
#include<stdio.h>
/*   ʹ��sprintf�����Ľ���
Ϊ��ʹ��sprintf,���Ʋ���ϵͳ�в��������printf�������������sprintf����ʹ�ã���Ϊ
sprintf���ǰ���ָ����ʽ�����ֻ�ǽ����������Ϊ�ַ���д���ڴ��У�
����õ�sprintf�������Ǳ���ʹ�õ���ΪGO��C�������������ĺ��������������ߵľ��������
�ܹ���ʹ�ò���ϵͳ���κι��ܣ���printf�Ļ�������������Ҫʹ�ò���ϵͳ�Ĺ��ܣ���sprintfֻ��Ҫ���ڴ���в���

*/

/* ����C����������һ�������ڱ���ļ���
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
//void write_mem8(int addr, int data);
	һ���ɻ�
	������ôд���Ǻ�������ȴ����{}������;����ʾ�����ڱ���ļ��У��Լ�ȥ��
	�����Ҿ��ò��ǣ������Makefile�������涨���ļ��ɣ���Ȼ���������Щ������nas�ļ���ĺ�������ͨ��makefi
	����ô���ļ��и���һ�����Ӱɣ�



//����д��ͬһ��Դ�ļ��������ڶ���ǰʹ�ã����Ǳ�����������һ��
void init_palette(void);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void set_palette(int start, int end, unsigned char *rgb);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, 
	int pysize, int px0, int py0, char *buf, int bxsize);
	Ϊʲô�����������gdb_init? 
	void init_gdtidt(void);
	void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
	void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

 */

/*	ָ��Ľ��ͣ�
	ָ�뷽��һ��
	for(i = 0xa0000; i <= 0xaffff; i++)
	{
		//write_mem8(i, 15); // MOV BYTE [i],15 
		//write_mem8(i,i & 0x0f); // MOV BYTE [i],15 
		p = (char *)i ;//�����ַ
		*p = i & 0x0f;
		//���Դ���write_mem8(i,i & 0x0f);
	}
	��������
	p = (char *) 0xa0000;//����ַ������ֵ
	
	for(i = 0; i <= 0xffff; i++)
	{
		*(p + i) = i & 0x0f;
	}
	
*/

/* ������ɫ
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

/*	�ṹ���˵��
# ��ͨ�Ľṹ��ʹ�÷���
�ṹ��ĺô��ǿ������������������ֶ�����һ���ԵĴ�����
func(abc);
���û�нṹ�壬��ֻ�ܽ���������һ��һ���Ĵ�����
func(scrnx, scrny, vram, ....);
���Ժܶ�ʱ�򶼻Ὣ��ĳ����������ݶ����ɵ�һ���ṹ�����棬�����ͷ������
����������ɷ������ˣ��ͺܶ��鷳
�ṹ���ڲ��ı�����Ҫ����д�ģ��ڽṹ��������ĺ����һ���㣨.����Ȼ����д�����������ǹ���


struct BOOTINFO abc:
abc.scrnx = 320;
abc.scrny = 200;
abc.vram  = 0xa0000;


��һ����ʹ��ָ�롣5.2����ʹ�÷����������������£�
���������� *ָ�������;��char *p;)
�����εı��������� struct BOOTINFO, ��������binfo������д��������ʽ��
struct BOOTINFO *binfo;

��Ϊbinfo��ʾָ���������ַ��4���ֽ�����ʾ������binfohi4�ֽڱ�����
��Ϊ��ָ�����������Ӧ�����ȸ�ָ�븳ֵ������Ͳ�֪��Ҫ�������д�ˣ�
����д���������������� binfo = 0x0ff0; ����Ϊ��warning�����Լ�������ת��
binfo = (struct BOOTINFO *)0x0ff0;

�趨��ָ���ַ����12���ֽڵĽṹ����������OK�ˣ��������ǿ��Բ���
ֱ��ʹ���ڴ��ַ������ʹ��*binfo����ʾ����ڴ��ַ��12�ֽڵĽṹ�塣����"char *p"
�е�*p��ʾp��ַ��1�ֽ���ͬ������
Ҫ���ʾ�ṹ�����ABC�е�scrnxʱ������abc.scrnx,������ƣ�������(*binfo).scrnx��ʾ 


*/
/* ת�Ƶ���bootpack��h
	struct BOOTINFO
{	
	char cyls, leds, vmode, reserve;	//һ�ֽڵ�cyls��leds��vmode��reserve����
	short scrnx, scrny;					//
	char *vram;
}; */

/* 	�Ƶ���dsctbl.c��
	struct SEGMENT_DESCRIPTOR
{
		SEGMENT_DESCRIPTOR�ṹ��
		SEGMENT_DESCRIPTOR�д��GDT��8�ֽ����ݣ�
		����CPU������Ϊ������д�ɽṹ�����ʽ
	
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR
{	
	GATE_DESCRIPTOR�ṹ��
		GATE_DESCRIPTORҲ����CPU������Ϊ������д���˽ṹ�����ʽ


	
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
	/*	ǰ���õ��ı�������
		int i;//i��һ��32λ����
		char *p;//����p������BYTE2�͵�ַ,��BYTE[...]�õĵ�ַ
		char *vram;//����vram������BYTE2�͵�ַ,��BYTE[...]�õĵ�ַ
		int xsize, ysize;
	
		short *binfo_scrnx, *binfo_scrny;
		int  *binfo_vram;//+�����Ѿ���struct������������
	//struct BOOTINFO *binfo;//���������� *binfo��struct BOOTINFO�Ľṹ�����*/
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;//����ǰ�����һ��������binfo = 0x0ff0�ϲ���
	//extern char hankaku[4096];
	char  mcursor[256], s[40];
	int mx, my;
	
	
	/*	֮ǰ��ʾA�ַ���16λ
		static char font_A[16] = {
			0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
			0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00, 
		};
	*/
	/*	��ͷ�Ǻ�
		ʹ�ü�ͷ�Ǻţ����Խ�"xsize = (*binfo).scrnx;" д��"xsize = binfo->scrnx"
		binfo = (struct BOOTINFO *) 0X0ff0;
		vram = (char *) *binfo_vram;//����ַ��ֵ��vram
		xsize = (*binfo).scrnx;
		ysize = (*binfo).scrny;
	*/
	init_gdtidt();
	init_palette();//�趨��ɫ��
	init_screen(/*vram = */binfo->vram, /*xsize =*/ binfo->scrnx, /*ysize = */binfo->scrny);
	mx = (binfo->scrnx - 16) / 2;//������Ļ���ĵ�����
	my = (binfo->scrny - 28 - 16) /2;
	/*	֮ǰ����ʾ��Ļ�ϴ��ABC 123 xyz��haribote os��sprintf��������ʾ
		//putfonts8_asc(binfo->vram, binfo->scrnx,  8,   8,COL8_FFFFFF, "ABC 123 xyz");
		//putfonts8_asc(binfo->vram, binfo->scrnx,  31, 31,COL8_C6C6C6, "Haribote OS");
		//putfonts8_asc(binfo->vram, binfo->scrnx,  31, 54,COL8_FFFFFF, "Haribote os");
		//sprintf( s , "scrnx = %d", binfo-> scrnx);
		//putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF,s);
	*/
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "( %d ,%d )", mx, my);//��ʾ����Ļ�ϵ��ַ�
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);//�ַ�����
	
	
	/* ָ�븳ֵ
		p = (char *) 0xa0000;//����ַ��ֵ��p
	*/
	/*	screen�����ľ���
		ԭ��screen��������ļ��������Ƿ�������ģ����洴����һ��screen�����Ͱ����ŵ�screen����
		for(i = 0; i <= 0xffff; i++)
		{
			p[i] = i & 0x0f;
		}*/
	
	
	
	for(;;)
	{
		io_hlt();						/*ִ��naskfunc���_io_hlt  */
	}
	
	return;

}
/* �Ƶ���graphic.c
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
	//C�����е�static char���ֻ���������ݣ��൱�ڻ���е�DBָ��
}
void set_palette(int start, int end, unsigned char *rgb)
{
	int i,eflags;
	eflags = io_load_eflags();									//��¼�ж���ɱ�־��ֵ
	io_cli();												  //���ж���ɱ�־��Ϊ0����ֹ�ж�
	io_out8(0x03c8,start);
	for (i = start; i <= end; i++)						   		//��ָ��װ�����洫�����ݵĺ���
	{
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);										//��ԭ�ж���ɱ�־
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
  //boxfill8(vram, x, COL8_FFFFFF, 	3, 	y-24,  2,  y-27);//Ϊʲô��һ��û�ˣ�
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
	//׼�����ָ��(16*16)
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
/*	�����������ͣ�
	����ı����ܶ࣬����VRam��vxsize�ǹ���VRam����Ϣ�����ǵ�ֵ�ֱ���0xa0000��320 
	pxsize��pysize����Ҫ��ʵ�� ͼ�εĴ�С�����ָ��Ĵ�С��16*16������������ֵ����16
	px0��py0ָ��ͼ���ڻ����ϵ���ʾλ�á�����buf��bxsize�ֱ�ָ��ͼ�εĴ�ŵ�ַ��ÿһ�к��е���������
	bxsize��pxsize������ͬ����Ҳ��ʱ������벻ͬ��ֵ�����Ի���Ҫ�ֱ�ָ��������ֵ
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
/* 	�Ƶ���dsctbl.c
	void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
		����gdt�ĺ���
		����gdt����ֵ0x00270000,����˵Ҫ��0x270000~0x27ffff��ΪGDT��
		�����ַ������ѡ�ģ�û�����⺬�塣
	
	
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;
		idt������
		idt���趨����0x26f800~0x26ffff,����ڴ��м����bootpack.h
		�����asmhead.nas �ļ��ᴦ����Щ��Ϣ��
	
	
	int i;
	GDT�ĳ�ʼ��
	for (i = 0; i < 8192; i++)
	{
		
		set_segmdesc(gdt + i, 0, 0, 0);
			��ѭ���ͺ��������Ľ���
			gdt��0x2700000,i��0��ʼ��ÿ�μ�1��֪��8191������һ����gdt+i���Ҳֻ����0x271fff��
			����ʵ�����ǣ�C�����н���ָ��ļӷ�����ʱ���ڲ��������ų˷����㡣����gdt�Ѿ�����
			Ϊָ�룬ָ��SEGMENT_DESCRIPTOR����һ��8�ֽڵĽṹ�壬������gdt���һ������ǵ�ַ
			����8��
			������forѭ����������˶�����8192���ε��趨�������ǵ����ޣ�limit��ָ�ε��ֽ���-1��
			��ַ��base��������Ȩ�޶���Ϊ0��
			
		
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
		���������������Ľ��ͣ�
	
		��������ǶԶκ�Ϊ1��2�������ν��е��趨���κ�Ϊ1��2�������ν��е��趨��
		�κ�Ϊ1�ĶΣ�����ֵΪ0xffffffff����С������4GB������ʾ����CPU���ܹ����ȫ���ڴ汾��
		�κ�Ϊ2�ĶΣ���С��512KB����ַ��0x280000����������Ϊ��bootpack.hrb��׼���ģ�
		������Σ��Ϳ���ִ��bootpack.hrb. ��Ϊbootpack.hrb����ORG 0 Ϊǰ�ᷭ��ɵĻ�������
		
	
	
	load_gdtr(0xffff, 0x00270000);//������Ϊ���ճ��棬C�����в��ܸ�GDTR��ֵ������Ҫ������������
	IDT�ĳ�ʼ��
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
			����ar = ar|0x8000��ʡ�Ա�����ʽ��ͬ���������limit /= 0x1000;
			"|"��or�����
		 
		limit /= 0x1000;
	}
	sd->limit_low	 = limit & 0xffff;
	sd->base_low	 = base & 0xffff;
	sd->base_mid	 = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high	 = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high	 = (base >> 24) & 0xff;
	
		����>>�Ľ���    ������
		>>������λ��������ܸ��ӣ�û�㶮
	
	
	
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















