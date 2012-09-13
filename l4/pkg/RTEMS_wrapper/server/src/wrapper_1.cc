/*
 * This is my first attempt to create a wrapper for RTEMS to get in on L4 via 
 * the vCPU interface.
 *
 * Maintainer:	philipp.eppelt@mailbox.tu-dresden.de
 * Date:	08/2012
 * Purpose: The wrapper loads an elf file, provided as 2nd argument into its
 *	    addressspace and ties it to a vcpu task.
 * 
 */

#define DEBUG 1

/* C includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* L4 includes */
#include <l4/sys/task>
#include <l4/sys/irq>
#include <l4/sys/vcpu.h>
#include <l4/sys/scheduler>
#include <l4/sys/ipc.h>
#include <l4/sys/thread>
#include <l4/sys/utcb.h>
#include <l4/sys/types.h>
#include <l4/sys/factory>
#include <l4/sys/kdebug.h>
#include <l4/vcpu/vcpu>
#include <l4/vcpu/vcpu.h>
#include <l4/re/mem_alloc>
#include <l4/re/debug>
#include <l4/re/util/cap_alloc>
#include <l4/re/env>
#include <l4/util/util.h>
#include <l4/util/elf.h>
#include <l4/util/port_io.h>
#include <l4/libloader/elf>
#include <uclibc/fcntl.h>
#include <l4/RTEMS_wrapper/wrapper_1.h>
#include <l4/RTEMS_wrapper/shared.h>
// goos framebuffer and font includes
#include <l4/re/util/video/goos_fb>
#include <l4/libgfxbitmap/bitmap.h>
#include <l4/libgfxbitmap/font.h>


using L4Re::chksys;
using L4Re::chkcap;

using namespace L4;
using namespace L4Re;

static L4::Cap<L4::Task> vcpu_task;
static L4vcpu::Vcpu *vcpu;
static L4::Cap<L4::Irq> timerIRQ;
static L4::Cap<L4::Thread> vcpu_cap;
static l4_vcpu_state_t *vcpuh;

static char thread_stack[8 << 10];
static char hdl_stack[8 << 10];
static char timer_stack[8<<10];

static unsigned long fs, ds;


/* Multiboot structure to provide lower and upper memory */
typedef struct multiboot
{
  long flags;
  long mem_lower;
  long mem_upper;
} multiboot_structure;



/* setup_user_state:
 * save the current fs, ds and set the vcpu segments */
static void
setup_user_state( l4_vcpu_state_t *vcpu)
{
  asm volatile( "mov %%fs, %0" : "=r"(fs) );
  asm volatile( "mov %%ds, %0" : "=r"(ds) );
  vcpu->r.gs = ds;
  vcpu->r.fs = fs;
  vcpu->r.es = ds;
  vcpu->r.ds = ds;
  vcpu->r.ss = ds;
#if !DEBUG
  enter_kdebug( "setup user state" );
#endif
}



/* handler_prolog:
 * recover the saved host fs & ds */
static void
handler_prolog()
{
#if !DEBUG
  printf( "fs: %x\n", fs);
  enter_kdebug( "handler prolog" );
#endif
  asm volatile( 
      " mov %0, %%es \t\n"
      " mov %0, %%ds \t\n"
      " mov %1, %%fs \t\n"
      : : "r"(ds),"r"(fs) );
}


#if 0
// deprecated, as we use the serial console  or redirect output to l4_printf

void
initVideo( sharedvars_t *sharedVar )
{
//  printf( "Addr sharedVar: %p, Value: %p, 
  Cap<Video::Goos> fb_cap = Env::env()->get_cap<Video::Goos>( "goosfb" );

  Util::Video::Goos_fb fb( fb_cap );
  l4_size_t ds_size = fb.buffer()->size();
  l4_addr_t ds_start;
  long err = Env::env()->rm()->attach( &ds_start, ds_size, Rm::Search_addr, fb.buffer() );

  if( err )
    printf( "Could not attach goos_fb ds: %li\n", err );

  Video::View::Info info;
  err = fb.view_info( &info );
  if(err )
    printf ("Failed to obtain view info: %li\n", err );

  if( 0 != gfxbitmap_font_init() )
    printf( "gfxbitmap_font_init failed. \n" );
  

  // I assume bitmapBaseAddr is the start of the frambuffer memory
  sharedVar->bitmapBaseAddr = reinterpret_cast<unsigned long*>(ds_start);
  // I assume ioCrtBaseAddr is the start of the framebuffer memory, we'll see
  sharedVar->ioCrtBaseAddr = ds_start;
  unsigned defaultFontHeight = gfxbitmap_font_height ( GFXBITMAP_DEFAULT_FONT );
  unsigned defaultFontWidth = gfxbitmap_font_width (GFXBITMAP_DEFAULT_FONT );
  sharedVar->linesPerPage = info.height / defaultFontHeight;
  sharedVar->columnsPerPage = info.width / defaultFontWidth;
  memset( (void*)ds_start, 6, ds_size );
#if DEBUG
  printf( "ds_start: %lx, bitmapBaseAddr: %p, ioCrtBaseAddr: %lx\n", 
      ds_start, sharedVar->bitmapBaseAddr, sharedVar->ioCrtBaseAddr ); 
  printf( "lPP: %lu , cPP %lu\n", 
      sharedVar->linesPerPage, sharedVar->columnsPerPage);
#endif
}
#endif



static void
handler()
{ /* This handler is invoked, if the guest system suffers a page fault or an
     interrupt. The handler checks the entry reason, and replies with an 
     apropriate action. */
  printf("Hello Handler\n");
//  vcpu->print_state("State:");
#if DEBUG
  enter_kdebug("handler entry");
#endif
  handler_prolog();
  vcpu->state()->clear( L4_VCPU_F_EXCEPTIONS );
  printf("vcpu->ip: %lx\n", vcpu->r()->ip);
  
  /* checking the entry reason */
  if( vcpu->is_page_fault_entry() )
  {
    printf("page fault entry\n");
    l4_umword_t pfnum = vcpu->r()->pfa;
    printf("pfa: %lx\n",pfnum);
    /* where do I get the snd_base? */
    vcpu_task->map( L4Re::This_task, 
	l4_fpage( pfnum, L4_PAGESIZE, L4_FPAGE_RWX ),
	 0);
    printf("Handler: Page mapped\n");
    /* copied out of vcpu example: why state add? */
    vcpu->saved_state()->add( L4_VCPU_F_PAGE_FAULTS );
    printf("Handler: saved_state added\n");
  }
  else if( vcpu->is_irq_entry() )
  {
    printf("irq entry!\n" );
    switch( vcpu->i()->label )
    {
      case(9000):
	   printf("Triggered Timer IRQ\n");
	   break;
      default:
	   printf("Unrecognized IRQ\n");
	   printf("irq: %lx \n", vcpu->i()->tag.label() );
	   //call RTEMS bsp_interrupt_handler_dispatch( vcpu->i()->label() );
    }
  }
  else
  {
    vcpu->print_state("Handler Unknown Entry:");
  }

//  vcpu->print_state("resume ");
  L4::Cap<L4::Thread> self;
  self->vcpu_resume_commit( self->vcpu_resume_start() );

  printf("hier gehts nicht weiter\n");
  l4_sleep_forever();
}



void
starter( void )
{ /* This is the starting point for the vcpu task. It sets its task and 
     resumes immediatly. */

  printf("Hello starter\n");

  printf( "VCPU fs: %x, gs: %x \n", (unsigned int) vcpu->r()->fs, 
      (unsigned int) vcpu->r()->gs);

//  enter_kdebug( "VCPU starter" );

  L4::Cap<L4::Thread> self; 
  self->vcpu_resume_commit( self->vcpu_resume_start() );
  
  printf("sleep forever\n");
  l4_sleep_forever();

}



void
l4rtems_timer( unsigned long period = 1 )
{ /* This function triggers an IRQ to test the IRQ entry capability of the running 
     rtems guest application. The timer resolution is milliseconds.
     If no parameter is set, the default period is 1ms. */
  
  timerIRQ->attach( 9000, vcpu_cap );

  while(1)
  {
    timerIRQ->trigger();
    l4_sleep( period );
  }
}




l4_umword_t
load_elf( char *name, l4_umword_t *initial_sp )
{ /* The function loads the elf file by name into the address space of the 
     invoking task and returns the entry IP and SP of the loaded elf. */ 
  
  printf("Hello %s \n", name);
  struct stat64 st;
  int fd = open( name, O_RDONLY);
  int r = fstat64(fd, &st);
  if( r  == -1 )
    printf( "fstat failed: %i\n", r);
  void *e = mmap( 0, st.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, 
      MAP_PRIVATE, fd, 0);
    
  Ldr::Elf_ehdr *ehdr = reinterpret_cast<Ldr::Elf_ehdr*>( e );
  if( !ehdr->is_valid() )
  {
    printf("ERROR: No valid elf!\n");
    l4_sleep_forever();
  }
  int num_phdrs = ehdr->num_phdrs();
  // there are two headers in the file, but we only need the first.
  if( num_phdrs > 1 )
  {
    printf("Code cannot handle more than one phdr! going to sleep\n");
//    l4_sleep_forever();
  }
/* save mapping information for the binary */
  l4_addr_t vaddr;
  unsigned long memsz = 0, filesz = 0, offset = 0;
  for( int idx = 0; num_phdrs--; idx++)
  {
    Ldr::Elf_phdr phdr= ehdr->phdr(idx);
    if( phdr.type() == PT_LOAD )
    {
      vaddr  = phdr.vaddr();
      memsz  = phdr.memsz();
      filesz = phdr.filesz();
      offset = phdr.offset();
    }
  }
  munmap( e, st.st_size);
  memsz += 0x1000;
  
  /* aquire capability, dataspace and mapping region */
  printf("phdr load construction site\n");
  L4::Cap<L4Re::Dataspace> mem_cap = L4Re::Util::cap_alloc.alloc<L4Re::Dataspace>();
  if( !mem_cap.is_valid() )
  {
    printf("ERROR: Dataspace cap invalid!\n");
    l4_sleep_forever();
  }
  long ds = L4Re::Env::env()->mem_alloc()->alloc( memsz, mem_cap );
  if( ds ) 
  {
    printf("ERROR: alloc returns %li \n",ds);
    l4_sleep_forever();
  }
  printf("DEBUG: vaddr %lx\n", vaddr);
  ds = L4Re::Env::env()->rm()->attach( &vaddr, memsz, L4Re::Rm::Eager_map, mem_cap );
  L4::Cap<L4Re::Debug_obj> d(L4Re::Env::env()->rm().cap());
  if( ds == -99 ) 
  {
    printf("ERROR: %lx not available\n", vaddr);	
    l4_sleep_forever();
  }
  else if( ds )
  {
    printf("ERROR: attach returns: %li\n",ds);
    l4_sleep_forever();
  }
  /* mmap the binary to the needed location */
  e = mmap( (void*)vaddr, memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
  ehdr = reinterpret_cast<Ldr::Elf_ehdr*>( e );
  Ldr::Elf_phdr *phdr = reinterpret_cast<Ldr::Elf_phdr*>((char*)ehdr + offset);
  /* take the mmaped elf, zero the .bss  */
  memset( (char*)phdr+filesz, 0, memsz-filesz );
  memcpy( (void*)vaddr, (void*)phdr, memsz);
  /* save the stack pointer for later use */
  *initial_sp = (l4_umword_t) vaddr+memsz-5;  

  d->debug(0);
  return ehdr->entry();
}



int
main( int argc, char **argv )
{ /* The main control structure, which sets up the environment for the 
      vcpu task and checks all necessary inputs are provided. At the end it 
      starts the vcpu and goes to sleep. It never exits. */
  
  printf( "Hello Wrapper!\n" );
  asm volatile (" mov %%fs, %0" : "=r"(fs) :: );
  printf( "fs: %lx\n", fs);
  /* 1st argument must be the RTEMS elf file */
  if( argc < 2 )
  {
    printf("ERROR: No elf provided!\n");
    return -1; 
  }

  /* open elf file and load it into memory */
  l4_umword_t initial_sp, entry;
  entry = load_elf( argv[1], &initial_sp );
  
  memset( hdl_stack, 0, sizeof(hdl_stack) );
  memset( thread_stack, 0, sizeof(thread_stack) );

  // new task
  vcpu_task = L4Re::Util::cap_alloc.alloc<L4::Task>();
  L4Re::Env::env()->factory()->create_task(vcpu_task, 
	        l4_fpage_invalid());

  // new thread/vCPU
  vcpu_cap = L4Re::Util::cap_alloc.alloc<L4::Thread>();
  l4_touch_rw(thread_stack, sizeof(thread_stack));
  L4Re::Env::env()->factory()->create_thread(vcpu_cap);
  _vcpu_cap = vcpu_cap.cap();

  // get memory for vCPU state
  l4_addr_t kumem = (l4_addr_t)l4re_env()->first_free_utcb;
  l4re_env()->first_free_utcb += L4_UTCB_OFFSET;
  l4_utcb_t *vcpu_utcb = (l4_utcb_t *)kumem;
  vcpu = L4vcpu::Vcpu::cast(kumem + L4_UTCB_OFFSET);

  // set entry IP + SP
  vcpu->entry_sp((l4_umword_t)hdl_stack + sizeof(hdl_stack));
  vcpu->entry_ip( (l4_umword_t)handler);

//  printf("VCPU: utcb = %p, vcpu = %p\n", vcpu_utcb, vcpu);
  
  setup_user_state(reinterpret_cast<l4_vcpu_state_t*> (vcpu));
  vcpu->saved_state()->set( 
       L4_VCPU_F_EXCEPTIONS
//      | L4_VCPU_F_IRQ
     /* | L4_VCPU_DEBUG_EXC*/ );
  
  
  // create multiboot structure
  // flags, lowerMem, upperMem [KB]
  multiboot_structure multi = { 1UL << 0, 512, 2048 };
  
  vcpuh = reinterpret_cast<l4_vcpu_state_t*> (vcpu);

  // create and fill shared variables structure
  sharedvars_t *sharedstruct = new sharedvars_t();
  sharedstruct->vcpu = vcpuh;
  sharedstruct->fd_in = stdin;
  sharedstruct->fd_out = stdout;
  
  // initialize the start registers
  vcpu->r()->ip = entry;
  vcpu->r()->sp = initial_sp;  
  vcpu->r()->ax = 0x2badb002;
  vcpu->r()->bx = (l4_umword_t) &multi; // pointer zur mutliboot struktur
  vcpu->r()->dx = (l4_umword_t) sharedstruct; // save it to edx, as it is not
					  // touched by the RTEMS start.S code
  //initVideo( sharedstruct );
  //    sharedstruct->bitmapBaseAddr, sharedstruct->ioCrtBaseAddr, sharedstruct->linesPerPage, sharedstruct->columnsPerPage );
#if DEBUG					  
  printf("ip: %lx, sp: %lx, bx: %lx\n",
     vcpu->r()->ip, vcpu->r()->sp, vcpu->r()->bx);
  printf( "vcpuh: %x, vcpu %x\n", (unsigned int) vcpuh, (unsigned int) vcpu);
  printf( "sharedVarStruct: %x \n", (unsigned int) &sharedstruct );
  printf( "sharedVarStruct: %x \n", (unsigned int) sharedstruct->vcpu );
#endif


  // give control to the vcpu 
  L4::Thread::Attr attr;
  attr.pager( L4::cap_reinterpret_cast<L4::Thread>( L4Re::Env::env()->rm() ) );
  attr.exc_handler( L4Re::Env::env()->main_thread() );
  attr.bind( vcpu_utcb, L4Re::This_task);
  chksys( vcpu_cap->control(attr), "control" );
  chksys( vcpu_cap->vcpu_control( (l4_addr_t) vcpu ), "enable VCPU" );
  chksys( vcpu_cap->ex_regs( (l4_umword_t) starter,
			     (l4_umword_t) thread_stack + sizeof(thread_stack),
			     0 ) );
  chksys( L4Re::Env::env()->scheduler()->run_thread( vcpu_cap, l4_sched_param(2) ) );

#if 1  
  // IRQ setup 
  timerIRQ = L4Re::Util::cap_alloc.alloc<L4::Irq>();
  L4Re::Env::env()->factory()->create_irq( timerIRQ );
  
  // create timer thread
  L4::Cap<L4::Thread> timer;
  timer->ex_regs( (l4_umword_t) l4rtems_timer,
		  (l4_umword_t) timer_stack + sizeof(timer_stack),
		  0 );
  L4Re::Env::env()->scheduler()->run_thread( timer, l4_sched_param(2) ); 
#endif
  
  l4_sleep_forever(); 
  return 0;
}


