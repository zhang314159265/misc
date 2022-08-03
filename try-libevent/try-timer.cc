#include <iostream>
#include <event2/event.h>
#include <event.h>

struct event* timer_ev;
struct timeval tv;

void callback(int h, short ev, void *data) {
  static int cnt = 0;
  std::cout << "Callback called " << ++cnt << std::endl;
  evtimer_add(timer_ev, &tv);
}

int main(void) {
  timer_ev = (struct event*) malloc(event_get_struct_event_size());
  event_base *base = event_base_new();
  evtimer_set(timer_ev, callback, nullptr);
  event_base_set(base, timer_ev);

  tv.tv_sec = 1;
  tv.tv_usec = 0;
  evtimer_add(timer_ev, &tv);
  event_base_dispatch(base);
  std::cout << "bye!" << std::endl;
  return 0;
}
