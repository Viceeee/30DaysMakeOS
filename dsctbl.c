#include "bootpack.h"
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


/*	对于struct SEGMENT_DESCRIPTOR的说明
	段的地址：地址是由32位来表示。这个地址在CPU里，被称为段的基址，所以这里使用了base这样一个变量名
	在这个结构体里base又分为low（2byte），mid（1），high（1）3段，合起来总共是32位，所以，这里只要按顺序
	分别填入相应的数值就行了，原理是程序中使用了移位运算符和AND运算符往各个字节里填入相应的数值
	要分3段的原因主要是为了与80286时代的程序兼容
	
	段上限：表示一个段有多少个字节，段上限最大是4GB，也就是一个32位的数值，如果直接放进去，这个数值本身就要占用
	4个字节，再加上基址（base），一共就要8个字节，这就把整个结构体占满了。这样一来就没哟有地方保存段的管理属性了
	因此段的上限就只能使用20位，所以，段上限最大也只能指定到1MB为止，明明有4GB却只能使用其中的1MB
	解决办法是在段的属性里设置了一个标志位，是G bit。这个标志位是1的时候，limit的单位不解释成字节，而解释成页
	1页是指4kb
	那么，4kb*1m = 4GB，所以可以指定4GB的段，G bit的G,是“granularity（粒度）”的缩写，是指单位的大小
	这20位的段上限要分别写到limit_low和limit_high里，看起来他们好像是总共有3个字节，24位，但实际上我们要把段属性
	写入limit_high的上4位里，所以还是20位
	
	12位的段属性：这个又称为“段的访问权属性”。在程序中用变量名access_right或者ar来表示，因为12位段中的高4位放在
	limit_high的高4位里，所以程序里有意把ar当做如下的16位来处理
	
		xxxx					0000      xxxxxxxx(其中x是0或1） 
		(前面4位是扩展访问权）    		  (低8位就是这8位）
				
	ar的高4位被称为“扩展访问权”，这高4位的访问属性在80286的时代还不存在，到386以后才可以使用。这4位是由“GD00"
	构成的，其中的G是指刚刚的G bit,D是指段的模式，1是指32位模式，0是指16位模式（所以这里的GD00是指16位）
	所以除了与运行80286的程序以外，都用D=1的模式
	
	ar的低8位从80286时代就已经有了，简单介绍一下：
	00000000(0x00): 未使用的记录表(descriptor table)
	10010010(0x92)：系统专用，可读写的段，不可执行
	10011010(0x9a)：系统专用，可执行的段，可读不可写
	11110010(0xf2)：应用程序用，可读写的段，不可执行
	11111010(0xfa)：应用程序用，可执行的段，可读不可写
	
	在32位模式下，CPU有系统模式和应用模式(其实就是ring0（指代前面的系统模式），ring1，ring2，ring3（应用模式），之类的）
	之分，操作系统等“管理用”的程序，和应用程序等“被管理”的程序，运行时的模式是不同的，
	
	比如在ring3模式下运行LGDT指令，CPU则对该指令不予以执行，并且发出警告，另外，当应用程序想要使用系统专用的段时，CPU也会停止执行
	并发出警告
	CPU到底是处于哪个状态，，取决于执行中的因公程序是位于访问权为0x9a的段还是位于访问权为0xfa的段



*/
 // struct SEGMENT_DESCRIPTOR
// {
	/*	SEGMENT_DESCRIPTOR结构体
		SEGMENT_DESCRIPTOR中存放GDT的8字节内容，
		是以CPU的资料为基础，写成结构体的形式
		这8个字节里面填充了段的大小，段的起始地址，段的管理属性(禁止写入，禁止执行，系统专用等)
	
	*/
	// short limit_low, base_low;//4 byte
	// char base_mid, access_right;//2 b
	// char limit_high, base_high;//2 b
// };

// struct GATE_DESCRIPTOR
// {	
	/*	GATE_DESCRIPTOR结构体
		GATE_DESCRIPTOR也是以CPU的资料为基础，写成了结构体的形式
	*/
	// short offset_low, selector;
	
	// char dw_count, access_right;
	// short offset_high;
// };

/* void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr); */ 

void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
	/*	变量gdt的含义
		变量gdt被赋值0x00270000,就是说要将0x270000~0x27ffff设为GDT。
		这个地址是随意选的，没有特殊含义。
	
	*/
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;
	/*	idt的作用
		idt被设定成了0x26f800~0x26ffff,这个内存中间会存放bootpack.h
		后面的asmhead.nas 文件会处理这些信息。
	
	*/
	int i;
	/*GDT的初始化*/
	for (i = 0; i < 8192; i++)
	{
		
		set_segmdesc(gdt + i, 0, 0, 0);
		/*	对循环和函数变量的解释
			gdt是0x2700000,i从0开始，每次加1，知道8191，这样一来，gdt+i最大也只能是0x271fff。
			但是实际上是：C语言中进行指针的加法运算时，内部还隐含着乘法运算。变量gdt已经声明
			为指针，指向SEGMENT_DESCRIPTOR这样一个8字节的结构体，所以往gdt里加一，结果是地址
			加了8。
			因此这个for循环语句就完成了对所有8192个段的设定，将他们的上限（limit，指段的字节数-1）
			基址（base）、访问权限都设为0。
			
		*/
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
	/*	对上面两个函数的解释：
	
		以上语句是对段号为1和2的两个段进行的设定。段号为1和2的两个段进行的设定。
		段号为1的段，上限值为0xffffffff即大小正好是4GB，它表示的是CPU所能管理的全部内存本身。
		段号为2的段，大小是512KB，地址是0x280000。这正好是为了bootpack.hrb而准备的，
		用这个段，就可以执行bootpack.hrb. 因为bootpack.hrb是以ORG 0 为前提翻译成的机器语言
		
	
	*/
	load_gdtr(0xffff, 0x00270000);//这是因为依照常规，C语言中不能给GDTR赋值，所以要借助汇编的力量
	/*IDT的初始化*/
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
		/*	what the fuck about |=
			他是ar = ar|0x8000的省略表现形式，同理还有下面的limit /= 0x1000;
			"|"是or运算符
		*/ 
		limit /= 0x1000;
	}
	sd->limit_low	 = limit & 0xffff;
	sd->base_low	 = base & 0xffff;
	sd->base_mid	 = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high	 = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high	 = (base >> 24) & 0xff;
	
	/*	关于>>的解释    ？？？
		>>是右移位运算符，很复杂，没搞懂
	
	
	*/
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
