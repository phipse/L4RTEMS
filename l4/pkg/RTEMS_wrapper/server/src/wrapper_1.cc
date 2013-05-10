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
#include <map>

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
#include <l4/util/atomic.h>
#include <l4/util/elf.h>
#include <l4/util/port_io.h>
#include <l4/libloader/elf>
#include <uclibc/fcntl.h>
#include <l4/RTEMS_wrapper/wrapper_1.h>
#include <l4/RTEMS_wrapper/shared.h>
#include <contrib/libio-io/l4/io/io.h>

#include <l4/RTEMS_wrapper/timer_IPC.h>


using L4Re::chksys;
using L4Re::chkcap;

using namespace L4;
using namespace L4Re;

static L4::Cap<L4::Task> vcpu_task;
static L4vcpu::Vcpu *vcpu;
static L4::Cap<L4::Thread> vcpu_cap;
static l4_vcpu_state_t *vcpuh;

static char thread_stack[8 << 10];
static char hdl_stack[8 << 10];

static unsigned long fs, ds;

#ifdef __cplusplus
extern "C" {
#endif

unsigned long __l4_external_resolver(void);

void do_resolve_error(const char *funcname);
void do_resolve_error(const char *funcname)
{
	printf("Symbol '%s' not found\n", funcname);
	enter_kdebug("Symbol not found!");
}

#ifdef __cplusplus
}
#endif


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


void
starter( void )
{ /* This is the starting point for the vcpu task. It sets its task and 
     resumes immediatly. */

  printf("Hello starter\n");

  printf( "VCPU fs: %x, gs: %x \n", (unsigned int) vcpu->r()->fs, 
      (unsigned int) vcpu->r()->gs);

  enter_kdebug( "VCPU starter" );

  L4::Cap<L4::Thread> self; 
  self->vcpu_resume_commit( self->vcpu_resume_start() );
  
  printf("sleep forever\n");
  l4_sleep_forever();

}



l4_umword_t
load_elf( char *name, l4_umword_t *initial_sp )
{ /* The function loads the elf file by name into the address space of the 
     invoking task and returns the entry IP and SP of the loaded elf. */ 
  
  printf("Hello %s \n", name);
  struct stat64 st;
  int fd = open( name, O_RDONLY);
  int r = lstat64( name , &st);
  if( r  == -1 )
  {
    printf( "fstat failed: %i\n", r);
    perror( "fstat errno: " );
  }
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
    printf("Code cannot handle more than one phdr! undefined behaviour!\n");
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
      break;
    }
  }
  munmap( e, st.st_size);
  // size for stack and heap
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


// Stores the successfully aquired capabilities by IRQ Number
// Heap storage leads to memory clashes between the elf loader and this.
//static std::map< unsigned, Cap<Irq> > *irqCaps = new std::map< unsigned, Cap<Irq> >();
static std::map<unsigned, Cap<Irq> > irqCaps;

void
startTimerService( void )
{
  L4::Cap<L4::Irq> timerIRQ = L4Re::Util::cap_alloc.alloc<L4::Irq>();
  if( l4_msgtag_has_error(L4Re::Env::env()->factory()->create_irq( timerIRQ )) )
    printf( "	  factory irq create foo\n" );
  if( !timerIRQ.is_valid() ) 
  {
    printf( "timerIRQ cap is not valid! aborting!\n" );
    return;
  }
  timer_init(vcpu_cap, timerIRQ );
  // 0 = irqNbr
  irqCaps.insert( std::pair< unsigned, Cap<Irq> >(0, timerIRQ) );
}


int
main( int argc, char **argv )
{ /* The main control structure, which sets up the environment for the 
      vcpu task and checks all necessary inputs are provided. At the end it 
      starts the vcpu and goes to sleep. It never exits. */

/*  startTimerService();
  l4rtems_requestIrq(0);

  while( true ) {
    l4_sleep(1000);
    printf( "Current Time: %llu\n", l4re_kip()->clock );
  }
*/
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
  l4re_env()->first_free_utcb += L4_UTCB_OFFSET;
  printf( "vcpu utcb: %p\n", vcpu_utcb );
  printf( "vcpu: %p\n", vcpu );

  // set entry IP + SP
  vcpu->entry_sp((l4_umword_t)hdl_stack + sizeof(hdl_stack));
  vcpu->entry_ip( 0);

//  printf("VCPU: utcb = %p, vcpu = %p\n", vcpu_utcb, vcpu);
  
  setup_user_state(reinterpret_cast<l4_vcpu_state_t*> (vcpu));
  vcpu->saved_state()->set( 
       L4_VCPU_F_EXCEPTIONS 
       | L4_VCPU_F_IRQ
     /* | L4_VCPU_DEBUG_EXC*/ );
  
  
  // create multiboot structure
  // flags, lowerMem, upperMem [KB]
  multiboot_structure multi = { 1UL << 0, 512, 2048 };
  
  vcpuh = reinterpret_cast<l4_vcpu_state_t*> (vcpu);

  // create and fill shared variables structure
  sharedvars_t *sharedstruct = new sharedvars_t();
  sharedstruct->vcpu = vcpuh;
  sharedstruct->ufs = fs;
  sharedstruct->uds = ds;
  sharedstruct->logcap = L4Re::Env::env()->get_cap<void>( "moes_log" ).cap();
  sharedstruct->l4re_env = l4re_env();
  sharedstruct->external_resolver =  __l4_external_resolver;

  // initialize the start registers
  vcpu->r()->ip = entry;
  vcpu->r()->sp = initial_sp;  
  vcpu->r()->ax = 0x2badb002;
  vcpu->r()->bx = (l4_umword_t) &multi; // pointer zur mutliboot struktur
  vcpu->r()->dx = (l4_umword_t) sharedstruct; // save it to edx, as it is not
					  // touched by the RTEMS start.S code
#if DEBUG					  
  printf("ip: %lx, sp: %lx, bx: %lx\n",
     vcpu->r()->ip, vcpu->r()->sp, vcpu->r()->bx);
  printf( "vcpuh: %x, vcpu %x\n", (unsigned int) vcpuh, (unsigned int) vcpu);
  printf( "sharedVarStruct: %x \n", (unsigned int) &sharedstruct );
  printf( "sharedVarStruct_vcpu: %x \n", (unsigned int) sharedstruct->vcpu );
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


  startTimerService();
  
  
  
  l4_sleep_forever(); 
  return 0;
}


l4_fastcall bool
l4rtems_requestIrq( unsigned irqNbr )
{ /* Create IRQ object and attach it to the requested irqNbr.*/
  // request new capability

  Cap<Irq> newIrq; 
  printf("requestedNbr: %i\n", irqNbr );
//  enter_kdebug("requestIRQ");
  if( irqNbr == 0 )
  {
    Cap<Irq> timerIRQ = irqCaps.find(irqNbr)->second;
    if( l4_msgtag_has_error( timerIRQ->attach( 0, vcpu_cap ) ) )
      printf( "error attaching timerIRQ\n");
//    l4rtems_timer_start( 1000, 0xF  );
//    printf( "timer start returned\n");
//    printf( "current time: %llu\n", l4re_kip()->clock );
    return true;
  }
  else 
  {
    newIrq = Util::cap_alloc.alloc<Irq>();
    if( !newIrq.is_valid() )
    {
      printf( "newIrq cap invalid!\n\n" );
      return false;
    }

    long ret = l4io_request_irq( irqNbr, newIrq.cap() );
    if( ret )
    {
      printf( "l4io request irq failed: %li \n", ret );
      return false;
    }

    // attach vcpu thread to the IRQ
    l4_msgtag_t err = l4_irq_attach( newIrq.cap(), irqNbr, _vcpu_cap );
    if( err.has_error() )
    {
      printf( "IRQ attach failed! Flags: %x \n\n", err.flags() );
      return false;
    }
  }

  irqCaps.insert( std::pair< unsigned, Cap<Irq> >(irqNbr, newIrq) );
  return true; 
}

  
  
l4_fastcall void
l4rtems_detachIrq( unsigned irqNbr )
{
  //enter_kdebug("detatch irq" );
  // retrieve the irq capability
  // NOTE: shouldn't the [] operator do the same?
  Cap<Irq> oldCap = irqCaps.find(irqNbr)->second;
  irqCaps.erase( irqNbr );
  // Detach from irqNbr
  if( irqNbr == 0 )
  {
    l4rtems_timer_stop();
    oldCap->detach();
  }
  else
  {
    l4_msgtag_t err = l4_irq_detach( oldCap.cap() );
    if( err.has_error() )
    {
      printf( "detachIrq:: Detatch failed! Flags: %x\n\n", 
	  err.flags() );
    }
    if( l4io_release_irq(irqNbr, oldCap.cap() ) )
    {
      printf( "l4io_release_irq error" );
    }
  }
}

