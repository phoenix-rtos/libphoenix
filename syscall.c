int ph_syscall(int number, ...)
{
	int result;
	__asm__ volatile ("movl %0, %%eax; int $0x80;"
			  :"=a" (result)
			  :"" (number));
	return result;
}
