#include <stdint.h>
#include <kernel/isr.h>
#include <kernel/utils.h>

static irq_handler_t irqfuncs[256] = { 0 };
static isr_handler_t isrfuncs[256] = { 0 };

void register_irq_handler(uint8_t n, irq_handler_t handler) { irqfuncs[n] = handler; }
void unregister_irq_handler(uint8_t n) { irqfuncs[n] = 0; }

void register_int_handler(uint8_t n, isr_handler_t handler) { intfuncs[n] = handler; }
void unregister_int_handler(uint8_t n) { intfuncs[n] = 0; }


//this function is called in all the IRQs
//it will call the corresponding function in the irqfuncs array, as long as it's not NULL (and the interrupt is not spourious)
//if the called function returns a non-NULL pointer, that pointer will be used as a stack to switch the task
//this function correctly acknowledges normal and spourious hardware interrupts
void irqfunc(registers_t *regs)
{
	void *stack = NULL;
	irq_handler_t handler = irqfuncs[regs->int_no];
	if(pic_isnormalIRQ(regs->int_no))
	{
		if(handler)
			stack = handler(regs);
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
void fault_handler(uint32_t intnum, void *ctx)
{
	void *stack = NULL;
	isr_handler_t handler = isrfuncs[regs->int_no];
	if(handler)
		stack = handler(ctx);
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

	if (irqnum == 7 && isr & 0x80 || isr & 0x08)
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
