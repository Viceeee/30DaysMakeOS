/*	中断基础
	为了让鼠标指针移动，必须使用中断，要使用中断，则必须将GDT和IDT正确的初始化，为了正确的初始化，
	应该首先初始化PIC。
	
	PIC是“programmable interrupt controller”的缩写，PIC是将8个中断信号集合成一个中断信号的装置，
	PIC监视着输入管脚的8个中断信号，只要有一个中断信号进来，就讲唯一的输出管脚信号变为1，并告诉CPU
	设计者想要通过增加PIC来处理更多的中断信号，就把中断信号增加到了15个，并为此增设了2个PIC
	
	他们的线路如文件夹首页图所示，与CPU直连的叫master PIC，与master PIC相连的叫slave PIC，
	主PIC主要负责处理第0~7号的中断信号，从PIC负责处理第8-15号中断信号，只要主PIC不通知CPU，从PIC也传不给CPU

*/

void init_pic(void)
//PIC的初始化
{
	io_out8(PIC0_IMR,	0xff  );//禁止所有中断
	io_out8(PIC1_IMR,	0xff  );//同上
	
	io_out8(PIC0_ICW1,	0x11  );//边沿触发模式(edge trigger mode)
	io_out8(PIC0_ICW2,	0x20  );//IRQ0-7由int20-27接收
	io_out8(PIC0_ICW3,	1 << 2);//PIC由IRQ2连接
	io_out8(PIC0_ICW4,	0x01  );//无缓冲区模式
	
	io_out8(PIC0_ICW1,	0x11  );//边沿触发模式(edge trigger mode)
	io_out8(PIC0_ICW2,	0x28  );//IRQ0-7由int28-2F接收
	io_out8(PIC0_ICW3,	   2  );//PIC1由IRQ2连接
	io_out8(PIC0_ICW4,	0x01  );//无缓冲区模式
	
	io_out8(PIC0_IMR,	0xfb  );//11111011 PIC1以外全部禁止
	io_out8(PIC0_IMR,	0xff  );//11111111 禁止所有中断
	
	return ;
}
/*	以上是PIC的初始化程序
	从CPU角度讲，PIC是外部设备，CPU使用out指令进行操作，程序中的PIC0和PIC1，分别指master PIC和slave PIC
	

*/

