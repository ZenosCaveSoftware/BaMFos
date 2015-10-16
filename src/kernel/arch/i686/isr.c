#include <stdint.h>
#include <kernel/isr.h>
#include <kernel/utils.h>

void *(**irqfuncs)(void * ctx);
void *(**intfuncs)(void * ctx);
void *(**intfuncs_err)(void * ctx, uint32_t err_code);

//IRQs (the 16 IRQs the PIC has)
DEFIRQWRAPPER(0);
DEFIRQWRAPPER(1);
DEFIRQWRAPPER(2);
DEFIRQWRAPPER(3);
DEFIRQWRAPPER(4);
DEFIRQWRAPPER(5);
DEFIRQWRAPPER(6);
DEFIRQWRAPPER(7);
DEFIRQWRAPPER(8);
DEFIRQWRAPPER(9);
DEFIRQWRAPPER(10);
DEFIRQWRAPPER(11);
DEFIRQWRAPPER(12);
DEFIRQWRAPPER(13);
DEFIRQWRAPPER(14);
DEFIRQWRAPPER(15);
 
//exceptions without error code
DEFINTWRAPPER(0);//division by 0
DEFINTWRAPPER(1);//debug
DEFINTWRAPPER(2);//NMI
DEFINTWRAPPER(3);//breakpoint
DEFINTWRAPPER(4);//INTO
DEFINTWRAPPER(5);//BOUND
DEFINTWRAPPER(6);//invalid opcode
DEFINTWRAPPER(7);//coprocessor not available
DEFINTWRAPPER(9);//coprocessor segment overrun
DEFINTWRAPPER(16);//coprocessor error
 
//exceptions with error code
DEFINTWRAPPER_ERR(8);//double fault
DEFINTWRAPPER_ERR(10);//TSS error
DEFINTWRAPPER_ERR(11);//segment not present
DEFINTWRAPPER_ERR(12);//stack fault
DEFINTWRAPPER_ERR(13);//general protection fault
DEFINTWRAPPER_ERR(14);//page fault
 
//software interrupts
DEFINTWRAPPER(0x80);//system call

void register_irq_handler(uint8_t n, irq_t handler)
{
	if (n < 16) {
		irqfuncs[n] = handler;
	}
}

void register_int_handler(uint8_t n, isr_t handler)
{
	intfuncs[n] = handler;
}

void register_int_err_handler(uint8_t n, isr_err_t handler)
{
	intfuncs_err[n] = handler;
}
//this function is called in all the IRQs
//it will call the corresponding function in the irqfuncs array, as long as it's not NULL (and the interrupt is not spourious)
//if the called function returns a non-NULL pointer, that pointer will be used as a stack to switch the task
//this function correctly acknowledges normal and spourious hardware interrupts
void irqfunc(uint32_t irqnum, void *ctx)
{
	void *stack = NULL;
	if(pic_isnormalIRQ(irqnum))
	{
		if(irqfuncs[irqnum] != NULL)
			stack = irqfuncs[irqnum](ctx);
		pic_EOI(irqnum);
		//if(stack)
			//taskswitch(stack);
	}
	else
	{
		pic_EOI_spurious(irqnum);
	}
}
 
//this function is called in all the software interrupts, and in exceptions without error code
//it will call the corresponding function in the intfuncs array, as long as it's not NULL
//if the called function returns a non-NULL pointer, that pointer will be used as a stack to switch the task
void intfunc(uint32_t intnum, void *ctx)
{
	void *stack = NULL;
	if(intfuncs[intnum] != NULL)
		stack = intfuncs[intnum](ctx);
	//if(stack)
		//taskswitch(stack);
}
 
//this function is called in exceptions with error code
//it will call the corresponding function in the intfuncs_err array, as long as it's not NULL
//if the called function returns a non-NULL pointer, that pointer will be used as a stack to switch the task
void intfunc_err(uint32_t intnum, void *ctx, uint32_t errcode)
{
	void *stack = NULL;
	if(intfuncs_err[intnum] != NULL)
		stack = intfuncs_err[intnum](ctx, errcode);
	//if(stack)
		//taskswitch(stack);
}

uint8_t pic_isnormalIRQ(uint32_t irqnum)
{
	uint16_t isr;
	if (irqnum == 7 || irqnum == 15) {
		isr = pic_get_isr();
	}
	else
	{
		return 1;
	}

	if (irqnum == 7  && isr & 0x80 == 0x80 
		|| irqnum == 15 && isr & 0x08 == 0x08)
	{		
		return 1;
	}

	return 0;
}

void pic_EOI(uint32_t irqnum)
{
	if (irqnum >= 8)
	{
		outb(PIC2_COMMAND, PIC_EOI);
	}
	outb(PIC1_COMMAND, PIC_EOI);
}


void pic_EOI_spurious(uint32_t irqnum)
{
	if (irqnum == 15)
	{
		outb(PIC1_COMMAND, PIC_EOI);
	}
}
/* Helper func */
static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}
 
/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}
 
/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}
