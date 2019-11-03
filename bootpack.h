/* asmhead.nas*/
struct BOOTINFO
{
	//0x0ff0-0x0fff
	char cyls;// 启动区读硬盘读到何处为止
	char leds;//启动时键盘LED的状态
	char vmode;//显卡模式为多少位彩色
	char reserve;
	short scrnx, scrny;//画面分辨率
	char *vram;
};

#define ADR_BOOTINFO	0x00000ff0

/*naskfunc.nas*/
/*告诉C编译器，有一个函数在别的文件里*/
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
//void write_mem8(int addr, int data);
/*	一点疑惑
	书上这么写：是函数声明却不用{}，而用;，表示函数在别的文件中，自己去找
	但是我觉得不是，这个是Makefile里面所规定的文件吧，虽然它上面的这些个是在nas文件里的函数，是通过makefi
	在这么多文件中搞了一个链接吧？
*/
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);


/*graphic.c*/
void init_palette(void);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void set_palette(int start, int end, unsigned char *rgb);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, 
	int pysize, int px0, int py0, char *buf, int bxsize);
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
#define COL8_848484		15	//dark grey
	
/*dsctbl.c*/
	struct SEGMENT_DESCRIPTOR
{
	/* SEGMENT_DESCRIPTOR结构体
		SEGMENT_DESCRIPTOR中存放GDT的8字节内容，
		是以CPU的资料为基础，写成结构体的形式 */
	
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR
{	
	/* GATE_DESCRIPTOR结构体
		GATE_DESCRIPTOR也是以CPU的资料为基础，写成了结构体的形式
 */
	short offset_low, selector;
	
	char dw_count, access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a












