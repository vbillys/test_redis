#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"


void onMessage(redisAsyncContext *c, void *reply, void *privdata) {
  redisReply *r = (redisReply *) reply;
  if (reply == NULL) return;
  static bool _firsttime = 1;

  if (_firsttime){
    _firsttime=0;
  }
  else {
    redisContext *cs = (redisContext*)privdata;
    redisCommand(cs, "PUBLISH Cpp %s","Hello from Cpp");
  }

  if (r->type == REDIS_REPLY_ARRAY) {
    for (int j = 0; j < r->elements; j++) {
      printf("%u) %s\n", j, r->element[j]->str);
    }
  }
}

int main (int argc, char **argv) {
  if (argc < 2) {
    printf("error need channel to be specified\n");
    return 1;
  }
  signal(SIGPIPE, SIG_IGN);
  struct event_base *base = event_base_new();

  redisAsyncContext *c = redisAsyncConnect("pub-redis-13387.us-east-1-4.6.ec2.redislabs.com", 13387);
  redisContext *cs = redisConnect("pub-redis-13387.us-east-1-4.6.ec2.redislabs.com", 13387);
  if (c->err) {
    printf("error: %s\n", c->errstr);
    return 1;
  }

  redisLibeventAttach(c, base);
  redisAsyncCommand(c, onMessage, cs, "SUBSCRIBE %s",argv[1]);
  event_base_dispatch(base);
  return 0;
}
