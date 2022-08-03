#include <iostream>
#include <event2/event.h>
#include <unistd.h>

// TODO: in theory write can also block (or return EAGAIN) and we should create
// an event for write as well as read.

#define CHECK(cond) \
  if (!(cond)) { \
    throw std::runtime_error("Fail the check"); \
  }

struct worker_state {
  explicit worker_state(int _fd) : fd(_fd) { }
  int fd;
  char buf[4096];
  int cur = 0;
  event* read_event = nullptr;

  int buflen() const {
    return sizeof(buf);
  }

  void write_msg(int lineno, char* msg, int len) {
    char numbuf[8292];
    int reallen = snprintf(numbuf, sizeof(numbuf), "%3d: %.*s\n", lineno, len, msg);
    int nwritten = 0;
    while (nwritten < reallen) {
      int r = send(fd, numbuf + nwritten, reallen - nwritten, 0);
      CHECK(r > 0);
      nwritten += r; 
    }
  }

  void sendback() {
    buf[cur] = '\0';
    int lino = 1; // line number start from 1

    char *ptr = buf;
    while (true) {
      char *loc = strchr(ptr, '\n');
      if (loc) { // found '\n'
        write_msg(lino, ptr, loc - ptr);
        ptr = loc + 1;
      } else {
        write_msg(lino, ptr, strlen(ptr));
        break;
      }
      ++lino;
    }
  }
};

void release_state(worker_state* state) {
  close(state->fd);
  /*
   * NOTE the different between event_del and event_free.
   * event_del remove the event from the event base. It can be added back
   * later if needed. It's a counterpart to event_add.
   *
   * event_free implemented called event_del and release the memory for the
   * event. It's a counterpart for event_new
   */
  event_free(state->read_event);
  delete state;
}

void do_read(evutil_socket_t fd, short event, void *arg) {
  worker_state *state = (worker_state*) arg;
  char buf[1024];
  std::cout << "do_read called for fd " << fd << std::endl;
  int nread;
  while (true) {
    nread = recv(fd, buf, sizeof(buf), 0);
    if (nread <= 0) {
      break;
    }

    int restroom = state->buflen() - state->cur - 1; // leave 1 byte for '\0'
    int tocopy = std::min(restroom, nread);
    if (tocopy > 0) {
      memcpy(state->buf + state->cur, buf, tocopy);
      state->cur += tocopy;
    }
  }

  if (nread == 0) {
    state->sendback();
  } else {
    assert(nread < 0);
    if (errno == EAGAIN) {
      return;
    }

    // error happened.
    std::cout << "Error happens for fd " << fd << std::endl;
  }
 
  release_state(state);
}

void do_accept(evutil_socket_t listener, short event, void *arg) {
  auto base = (struct event_base*) arg;
  struct sockaddr_in sa;
  socklen_t slen = sizeof(sa);

  int fd = accept(listener, (struct sockaddr*) &sa, &slen);
  CHECK(fd >= 0);
  evutil_make_socket_nonblocking(fd);
  std::cout << "do_accept called, create fd " << fd << std::endl;

  // create worker event and its state
  worker_state* state = new worker_state(fd);
  struct event* read_event = event_new(base, fd, EV_READ | EV_PERSIST, do_read, state);
  state->read_event = read_event;
  event_add(read_event, nullptr);
}

int main(void) {
  std::cout << "hi" << std::endl;
  struct event_base *base;
  struct event *listener_event;
  evutil_socket_t listener;
  struct sockaddr_in sin;

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(1234);

  base = event_base_new();

  listener = socket(AF_INET, SOCK_STREAM, 0);
  CHECK(bind(listener, (struct sockaddr*) &sin, sizeof(sin)) == 0);
  CHECK(listen(listener, 32 /* backlog */) == 0);

  listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, base);
  event_add(listener_event, nullptr);
  event_base_dispatch(base);
  std::cout << "bye" << std::endl;
}
