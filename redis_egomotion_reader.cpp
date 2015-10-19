#include "hiredis/hiredis.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include "debug_print_common.hpp"

//#define REDIS_HOST "localhost"
#define REDIS_HOST "192.168.0.4"
#define REDIS_PORT 6379

#define THIS_IS_NOT_LIB_WITH_MAIN 1

int connectToRedis ( redisContext* & rcc ){

  rcc = redisConnectWithTimeout(REDIS_HOST, REDIS_PORT, {1,0});
  if (rcc == NULL || rcc->err) {
    if (rcc) {
      printf("Redis Connection error: %s\n", rcc->errstr);
      redisFree(rcc);
    } else {
      printf("Redis Connection error: can't allocate redis context\n");
    }
    return 0;
  } else
  {
    return 1;
  }
}

typedef struct EgoMotionFromAlphard{
  unsigned int timestamp;
  float vel_x;
  float z_angular_rate;
};

int getOdomRedis(redisContext * & c, EgoMotionFromAlphard & output)
{
	redisReply *reply_ts = (redisReply *)redisCommand(c, "HMGET %s tstamp", "EgoMotionFromRos");
	redisReply *reply_vel_x = (redisReply *)redisCommand(c, "HMGET %s vel_x", "EgoMotionFromRos");
	redisReply *reply_z_angular_rate = (redisReply *)redisCommand(c, "HMGET %s z_angular_rate", "EgoMotionFromRos");

	bool _success = false;
	static unsigned int recv_ts;

	
	if (reply_ts == NULL
	    || reply_vel_x == NULL
	    || reply_z_angular_rate == NULL
	   )
	{
	  debug_print_1("redisCommand err, probably server is closed connection....\n");
	  return 0;
	}

	if (reply_ts->element[0]->str != NULL
	    && reply_vel_x->element[0]->str != NULL
	    && reply_z_angular_rate->element[0]->str != NULL
	   )
	{

		unsigned int _ts = atol(reply_ts->element[0]->str);
		float _vel_x = atof(reply_vel_x->element[0]->str);
		float _z_angular_rate = atof(reply_z_angular_rate->element[0]->str);

		output.timestamp = _ts;
		output.vel_x = _vel_x;
		output.z_angular_rate = _z_angular_rate;

		if (_ts != recv_ts){
			_success = true;
			recv_ts = _ts;
		}
	}

	freeReplyObject(reply_ts);
	freeReplyObject(reply_vel_x);
	freeReplyObject(reply_z_angular_rate);

	if (_success){
		return 1;
	}else{
		return 0;
	}

}

int updateOdomRedis(EgoMotionFromAlphard & egomotion, bool auto_reconnect)
{
  static redisContext *rcc = NULL;

  while (1)
  {
    if (rcc != NULL){
	      if (!rcc->err){
		if (getOdomRedis(rcc, egomotion)){
		  debug_print_0("status %ld %f %f\n", egomotion.timestamp, egomotion.vel_x, egomotion.z_angular_rate);
		  usleep(10000);
		}
	      } else
	      {
		printf("Redis Connection error: %s\n", rcc->errstr);
		switch (rcc->err) {
		  case REDIS_ERR_EOF:
		    rcc = NULL;
		    if (auto_reconnect) {
		      printf("will reconnect!\n");
		    }
		    else return 0;
		    break;
		  case REDIS_ERR_IO:
		  case REDIS_ERR_PROTOCOL:
		  case REDIS_ERR_OTHER:
		  default:
		    printf("sorry can't recover Redis egomotion...bye!");
		    return 0;
		    ;
		}

	      }

    } else
    {
	      if(connectToRedis (rcc)){
		continue;
	      }
	      else {
		rcc = NULL;
		if (auto_reconnect) {
		  sleep(1.0);
		}
		else {
		  return 0;
		} 
	      }
    }
    
  }
}

#if THIS_IS_NOT_LIB_WITH_MAIN
int main (int argc, char **argv) {

  //GlobalDebugData::setDebugTest1Flag(false);
  
  EgoMotionFromAlphard egomotion;
  updateOdomRedis(egomotion, false);

}
#endif
