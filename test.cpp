#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libev.h"
#include "hiredis/adapters/libevent.h"
#include <ev.h>
#include <iostream>
#include <unistd.h>
#include <event2/event.h>
//#include "/usr/local/include/hiredis/adapters/ae.h"

#define TEST_EARLY 0
struct ev_loop *loop = ev_default_loop (0);

void onMessage(redisAsyncContext *c, void *reply, void *privdata) {
    redisReply *r = (redisReply *)reply;
    if (reply == NULL) return;

    if (r->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < r->elements; j++) {
            printf("%u) %s\n", j, r->element[j]->str);
        }
    }
    ev_unloop (EV_A_ EVUNLOOP_ALL);
}

void connectCallback(const redisAsyncContext *c, int status) {
 
  if ( status != REDIS_OK )
  {
    printf("Error: %s\n", c->errstr);
  }
  else
    printf("connected...\n");
}


void disconnectCallback(const redisAsyncContext *c, int status) {
  if (status != REDIS_OK) {
    printf("Error: %s\n", c->errstr);
  }
  printf("disconnected...\n");
}

void getCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = (redisReply *)r;
    if (reply == NULL) return;
    printf("argv[%s]: %s\n", (char*)privdata, reply->str);

    /* Disconnect after receiving the reply to GET */
    redisAsyncDisconnect(c);
}

  static void
  timeout_cb (EV_P_ struct ev_timer *w, int revents)
  {
    std::cout << "timeout" << std::endl;
    // this causes the innermost ev_loop to stop iterating
    //ev_unloop (EV_A_ EVUNLOOP_ONE);
    ev_unloop (EV_A_ EVUNLOOP_ALL);
  }

void cb_func(evutil_socket_t fd, short what, void *arg)
{
        char *data = (char *)arg;
        printf("Got an event on socket %d:%s%s%s%s [%s]",
            (int) fd,
            (what&EV_TIMEOUT) ? " timeout" : "",
            (what&EV_READ)    ? " read" : "",
            (what&EV_WRITE)   ? " write" : "",
            (what&EV_SIGNAL)  ? " signal" : "",
            data);
}

int main (int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
    
    ev_timer timeout_watcher;
    
    //struct event_base *base = event_base_new();
    //aeEventLoop *loop;
    
    struct event *ev1, *ev2;
    struct timeval five_seconds = {1,0};
    struct event_base *base = event_base_new();

    redisAsyncContext *c;
    c = (redisAsyncContext *)redisAsyncConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("error: %s\n", c->errstr);
        return 1;
    }
    if (base == NULL){
		printf("base NULL.!\n");
		return 1;
	}
    
    redisLibeventAttach(c, base);
    
    //redisLibevAttach(EV_DEFAULT_ c);
    
    redisAsyncSetConnectCallback( c,connectCallback);
    redisAsyncSetDisconnectCallback(c,disconnectCallback);
    
    //redisAsyncCommand(c, NULL, NULL, "SET key %b", argv[argc-1], strlen(argv[argc-1]));
    //redisAsyncCommand(c, getCallback, (char*)"end-1", "GET key");
    //redisAsyncCommand(c, onMessage, NULL, "SUBSCRIBE testtopic");

    /*for (int kk;kk<50;kk++){
	  //ev_timer_init (&timeout_watcher, timeout_cb, 0.1, 0.);
      //ev_timer_start (loop, &timeout_watcher);
      //
      std::cout << "fuck loop "<< kk << std::endl;
      //ev_loop(EV_DEFAULT_ 0);
      ev_loop( loop, 0);
      //usleep(100000);
      
    }*/
    
    ev1 = event_new(base, -1, EV_PERSIST, cb_func, event_self_cbarg());
    if (base == NULL){
		printf("ev1 NULL.!\n");
		return 1;
	}
    
    //event_add(ev1, &five_seconds);
    
    //redisAsyncCommand(c, onMessage, NULL, "SUBSCRIBE testtopic");
    //redisAsyncCommand(c, NULL, NULL, "SET key  %d", 1); 
     //event_base_dispatch(base);
    #if TEST_EARLY
    redisContext *rcc;
    rcc = redisConnectWithTimeout("localhost", 6379, {1,0});
    if (rcc == NULL || rcc->err) {
        if (rcc) {
            printf("Connection error: %s\n", rcc->errstr);
            redisFree(rcc);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        return(1);
    }
    #endif
    return 0;
}
