#include "../include/kernel.h"

void test_idt()
{
	__asm__("int $0x04");
}

void kernel_main(uint32_t mbaddr, uint32_t mbmagic,
		 kernel_mem_limits_t kmlimits, uint32_t boot_page_directory)
{
	multiboot_info_t *mbinfo = remap_multiboot_info(mbaddr);
	terminal_init();
	gdt_init();
	idt_init();

	// TODO: TSS init need to be fixed
	/*
	tss_init(5, 0x10, 0);
	uint32_t esp;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	tss_set_stack(0x10, esp);
	*/

	timer_init(100);
	keyboard_init();
	check_mboot_bootloader_magic(mbmagic);
	display_memory_info(mbinfo);
	display_kernel_mem_info(&kmlimits);

	paging_init(boot_page_directory);

	kmalloc_init(kmlimits.kernel_virtual_end -
			     (kmlimits.kernel_virtual_end % 4) + 4,
		     KERNEL_HEAP_SIZE);

#if TEST_IDT
	test_idt();
#endif

#if GUI_MODE
	set_mode(320, 200, 8);
	clear_screen_withcolor(0x3F);
	gui_draw_string(8, 16, " > GUI Mode Enabled", 0x2C);
#if VGA_DEMO
	int color = 0;
	while (1) {
		if (color > 256) {
			color = 0;
		}
		for (int i = 0; i < 99999999; i++) {
			// pseudo sleep()
		}
		gui_draw_string(8, 32, " > VGA Demo!", color);
		for (int i = 0; i < 99999999; i++) {
			// pseudo sleep()
		}
		color++;
	}

#endif

#endif
}
