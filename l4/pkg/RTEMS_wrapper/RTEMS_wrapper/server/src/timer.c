/* timer thread for RTEMS */

/*
void
l4rtems_timer(  )
{
  L4::Cap<L4::Irq> timer_irq = L4::Util::Cap_alloc.alloc<L4::Irq>();
  L4::Env::env()->factory()->create_irq( timer_irq );
  while(1)
  {
    timer_irq->trigger();
    sleep(10);
  }
}
*/
