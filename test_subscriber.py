import sys
import cmd
import redis
import threading

REDIS_HOST = 'localhost'
REDIS_PORT = 6379

import time

current_milli_time = lambda: int(round(time.time() * 1000))

def monitor():
    r = redis.Redis(REDIS_HOST, REDIS_PORT)#, YOURPASSWORD, db=0)

    channel = sys.argv[1]
    p = r.pubsub()
    p.subscribe(channel)

    last_millis = current_milli_time()
    no_of_received_messages = 0

    print 'monitoring channel', channel
    for m in p.listen():
	    # print 'o'
	    print len(str(m['data']))
	    current_millis = current_milli_time()
	    print current_millis-last_millis
	    print current_millis # current_milli_time()
	    last_millis=current_millis
	    print time.time()
	    print no_of_received_messages
	    no_of_received_messages=no_of_received_messages+1
    # print len(p.listen())


class my_cmd(cmd.Cmd):
    """Simple command processor example."""

    def do_start(self, line):
	    my_thread.start()

    def do_EOF(self, line):
	    return True


if __name__ == '__main__':
	if len(sys.argv) == 1:
		print "missing argument! please provide the channel name."
	else:
		monitor()
	    # my_thread = threading.Thread(target=monitor)
	    # my_thread.setDaemon(True)

	# my_cmd().cmdloop()
