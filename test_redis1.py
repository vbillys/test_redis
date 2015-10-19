import redis
import sys

REDIS_HOST = 'pub-redis-13387.us-east-1-4.6.ec2.redislabs.com'
REDIS_PORT = 13387



def monitor():
    r = redis.Redis(REDIS_HOST, REDIS_PORT)

    channel = sys.argv[1]
    p = r.pubsub()
    p.subscribe(channel)

    g_firsttime = True;

    print 'monitoring channel', channel
    for m in p.listen():
	print (m['data'])
	if g_firsttime:
		g_firsttime = False
	else:
		try:
			slist = m['data'].split()
			if len(slist) <2: 
				raise 
			first_int  = int(slist[0])
			second_int = int(slist[1])
			r.publish('Python',str(first_int + second_int))

		except:

			r.publish('Python','Hello from Python')



monitor()

    # while True:
	    # print (repr(r.blpop(channel)))
