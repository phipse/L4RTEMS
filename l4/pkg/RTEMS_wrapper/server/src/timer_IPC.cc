#include <l4/RTEMS_wrapper/timer_IPC.h>
#include <cstdio>

#include <l4/sys/factory>
#include <l4/sys/irq.h>
#include <l4/sys/capability>
#include <l4/sys/thread>
#include <l4/re/env>
#include <l4/sys/scheduler>
#include <l4/sys/err.h>
#include <l4/sys/utcb.h>
#include <l4/sys/types.h>
#include <l4/cxx/ipc_server>
#include <l4/cxx/ipc_stream>
#include <l4/re/cap_alloc>
#include <l4/re/util/cap_alloc>
#include <l4/re/error_helper>
#include <l4/util/util.h>
#include <l4/sys/ipc.h>
#include <l4/util/rdtsc.h>
#include <l4/sys/kdebug.h>

#include <type_traits>


static l4_timeout_t _timeout;



class L4rtems_timer : public L4::Server_object
{
  private:
    l4_cap_idx_t l4rtems_timer_cap;			// this cap
    
    Timer_ops current_state;				// timer state
    L4::Cap<L4::Irq> timerIRQ;
    l4_umword_t proto;					// proto recieve 
    unsigned long long next_timeout;			// next timeout
    l4_cpu_time_t period_length;			// period length
    L4::Cap<L4::Thread> vcpu_c;

  public:
    L4rtems_timer( L4::Cap<L4::Irq> tIRQ);
    ~L4rtems_timer();

    int retimeout( l4_msgtag_t tag, L4::Ipc::Iostream const &ios );
    int dispatch( unsigned long obj, L4::Ipc::Iostream &ios );
};



L4rtems_timer::L4rtems_timer( L4::Cap<L4::Irq> tIRQ)
{
  current_state = Timer_ops::L4RTEMS_TIMER_STOP;
  next_timeout = 0;
  period_length = 0;
  timerIRQ = tIRQ;
  
}

L4rtems_timer::~L4rtems_timer()
{}

L4::Cap<L4::Thread> vcpu_cap;
static L4rtems_timer *timer;

class Timer_hooks :
  //public L4::Ipc_svr::Ignore_errors,
  public L4::Ipc_svr::Compound_reply,
  public L4::Ipc_svr::Default_setup_wait
{
  private:
  protected:
  public:
    l4_timeout_t timeout() { 
	return _timeout; }
    void error( l4_msgtag_t tag, L4::Ipc::Iostream const &ios )
    {
      timer->retimeout( tag, ios );
    }
};


class L4rtems_timer_registry : public L4::Basic_registry
{
  public:
  static int dispatch(l4_umword_t label, L4::Ipc::Iostream &ios)
  {
    return timer->dispatch(label, ios);
  }
};

static L4::Server<Timer_hooks> *timer_server;
static L4rtems_timer_registry timer_registry;


int
L4rtems_timer::retimeout(l4_msgtag_t tag, L4::Ipc::Iostream const & )
{
//  printf( "retimeout\n");
  
  // 2. check if timeout occurred
  if( l4_ipc_error( tag, l4_utcb() ) == L4_IPC_RETIMEOUT ) // receive timeout
  {
    //	2.1. if timer running set next timeout point and wait again
    if( current_state == Timer_ops::L4RTEMS_TIMER_START )
    {
      int long err =  l4_ipc_error( this->timerIRQ->trigger(), l4_utcb() ); 
      if( err )
	printf( "IRQ trigger failed!\n %s\n", l4sys_errtostr(err) );

      next_timeout += period_length;
      _timeout = l4_timeout( L4_IPC_TIMEOUT_0, 
	  l4_timeout_abs(next_timeout, 1) );
      return L4_EOK;
    }
    //	2.2. if timer not running set timout to NEVER
    else if( current_state == Timer_ops::L4RTEMS_TIMER_STOP )
    {
      _timeout = L4_IPC_NEVER;
      return L4_EOK;
    }
    else
    { // something unforseeable happened, get back in a defined state
      // and try again.
      current_state = Timer_ops::L4RTEMS_TIMER_STOP;
      return -L4_EAGAIN;
    }
  }
  else
  { 
    printf( "an unexpected error occured in l4rtems_timer");
    return -L4_ENOENT;
  }
  
}



int L4rtems_timer::dispatch( unsigned long , L4::Ipc::Iostream &ios )
{
//  printf( "dispatching\n");
//  printf( "l4utcb: %p\n" , l4_utcb() );
  Timer_ops msg_op;
  l4_msgtag_t msg;
//  enter_kdebug("dispatch");
  ios >> msg;
  // 3. check the protocol
  if( l4_error( msg ) == static_cast<std::underlying_type<Protos>::type>(Protos::L4RTEMS_PROTO_TIMER) )
  {
    // 4. check the operation code
    ios.get( msg_op );
    current_state = msg_op;
    
    switch( msg_op )
    {
      //	4.1.  start operation: get the timer period and start the timer
      case Timer_ops::L4RTEMS_TIMER_START:
	printf( "msg_ops Timer Start\n");
	ios >> next_timeout;
	ios >> period_length;

	if( next_timeout == 0 )
	  // first interrupt fires after one period
	  next_timeout = l4re_kip()->clock + period_length; // micro sec
	else 
	  // first IRQ fires after the specified time
	  next_timeout += l4re_kip()->clock;

	printf( "current time: %llu\n", l4re_kip()->clock );
	printf( "next timeout: %llu\n", next_timeout );
	

	_timeout = l4_timeout( L4_IPC_TIMEOUT_0,
	    l4_timeout_abs( next_timeout, 1) );

	return L4_EOK;
	//	4.2.  stop operation: set timeout to NEVER
      case Timer_ops::L4RTEMS_TIMER_STOP:
	printf( "msg_ops Timer Stop\n");
//	printf( "current time: %llu\n", l4re_kip()->clock );
//	printf( "_timeout: %i\n",_timeout.p.rcv.t );

	_timeout = L4_IPC_NEVER;
	return L4_EOK;
	//	4.3.  other: send error msg
      default:
	printf( "L4rtems_timer: invalid opcode\n" );
	return -L4_ENOENT;
    }
  }
  else
    return -L4_EBADPROTO;
}


// ipc call order:
// Timer_ops, first, period 

#ifdef __cplusplus
extern "C" {
#endif

l4_fastcall void 
l4rtems_timer_start(  l4rtems_timer_t period,   // between two interrupts
		      l4rtems_timer_t first )   // first interrupt time
{
  // build an IPC with the start op and the period length
  L4::Ipc::Iostream ios(l4_utcb());
  ios.put(Timer_ops::L4RTEMS_TIMER_START);
  ios << first;
  ios << period;
  l4_msgtag_t msg = ios.call( timer_thread_cap.cap(), static_cast<std::underlying_type<Protos>::type>(Protos::L4RTEMS_PROTO_TIMER) );

  if( l4_ipc_error( msg, l4_utcb() ) )
    printf( "ERROR: while starting timer %li\n", l4_error(msg) );
}



// ipc call order:
// Timer_ops

l4_fastcall void 
l4rtems_timer_stop(  )//l4_cap_idx_t timer_cap )  // timer thread cap
{
  // build an IPC with the stop op
  L4::Ipc::Iostream ios(l4_utcb()) ;
  ios.put( Timer_ops::L4RTEMS_TIMER_STOP );
  l4_msgtag_t msg = ios.call( timer_thread_cap.cap(), static_cast<std::underlying_type<Protos>::type>(Protos::L4RTEMS_PROTO_TIMER) );
  
  if( l4_ipc_error( msg, l4_utcb() ) )
    printf( "ERROR: while stopping the timer %li\n", l4_error(msg) );
}

#ifdef __cplusplus
}
#endif


L4::Cap<L4::Irq> timerIRQ;

void
timer_loop( void )
{
  printf(" Hello Timer Loop\n " );
  timer_server = new L4::Server<Timer_hooks>( l4_utcb() ); 
  timer = new L4rtems_timer( timerIRQ );
  timer_server->loop( timer_registry );
}


void
timer_init( L4::Cap<L4::Thread> guest_cap, L4::Cap<L4::Irq> irq_cap )
{
  printf( "hello timer_init\n");
  vcpu_cap = guest_cap;
  timerIRQ = irq_cap;
  static char timer_stack[8<<10];

  // create new thread
  timer_thread_cap = L4Re::Util::cap_alloc.alloc<L4::Thread>(); 
  l4_touch_rw(timer_stack, sizeof(timer_stack));
  L4Re::Env::env()->factory()->create_thread(timer_thread_cap);
  
  // grab utcb for timer thread
  l4_addr_t kumem = (l4_addr_t)l4re_env()->first_free_utcb;
  l4re_env()->first_free_utcb += L4_UTCB_OFFSET;
  l4_utcb_t *utcb_time = (l4_utcb_t *)kumem;

  static L4::Thread::Attr attr_time;
  attr_time.pager( L4::cap_reinterpret_cast<L4::Thread>( L4Re::Env::env()->rm() ) );
  attr_time.exc_handler( L4Re::Env::env()->main_thread() );
  attr_time.bind( utcb_time, L4Re::This_task);

  timer_thread_cap->control( attr_time );
  timer_thread_cap->ex_regs( (l4_umword_t) timer_loop, // call server for looping
		  (l4_umword_t) timer_stack + sizeof(timer_stack),
		  0 );
  
  L4Re::Env::env()->scheduler()->run_thread( timer_thread_cap, l4_sched_param(4) ); 
}
