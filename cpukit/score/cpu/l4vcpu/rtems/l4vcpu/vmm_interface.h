// include cmd codes

static inline int
l4rtems_vmm_call( unsigned long cmd, 
    unsigned long* a, unsigned long* b, unsigned long* c, unsigned long* d )
{
  asm __volatile__ ( "hlt \n\t"
      : "=a"(cmd), "=b"(*a), "=c"(*b), "=d"(*c), "=S"(*d)
      : "a"(cmd), "b"(*a), "c"(*b), "d"(*c), "S"(*d)
      : "memory"
      );
  
  return cmd;
}


int
l4rtems_requestIrq(int irqNbr )
{
  unsigned long a = 0, b = 0, c = 0, d = 0;
  a = irqNbr;

  return l4rtems_vmm_call( 0x1, &a, &b, &c, &d );
}


int l4rtems_detachIrq( int irqNbr )
{
  unsigned long a = irqNbr, b = 0, c = 0, d = 0;
  
  return l4rtems_vmm_call( 0x2, &a, &b, &c, &d );
}
