library("rredis")
redisConnect(host = "pub-redis-13387.us-east-1-4.6.ec2.redislabs.com", port = 13387)
#redisSubscribe("yaya")
repeat {
  print("Inside Loop")
  print(redisBLPop("rc_out"))
}
