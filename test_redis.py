from twisted.internet import reactor, protocol, defer
from twisted.python import log

from txredis.client import RedisClient, RedisSubscriber

import sys

import numpy as np

REDIS_HOST = 'localhost'
REDIS_PORT = 6379


def getRedisSubscriber():
    clientCreator = protocol.ClientCreator(reactor, RedisSubscriber)
    return clientCreator.connectTCP(REDIS_HOST, REDIS_PORT)


def getRedis():
    clientCreator = protocol.ClientCreator(reactor, RedisClient)
    return clientCreator.connectTCP(REDIS_HOST, REDIS_PORT)


@defer.inlineCallbacks
def runTest():
    redis1 = yield getRedisSubscriber()
    redis2 = yield getRedis()

    # log.msg("redis1: SUBSCRIBE w00t")
    # response = yield redis1.subscribe("w00t")
    # log.msg("subscribed to w00t, response = %r" % response)

    log.msg("redis2: PUBLISH w00t 'Hello, world!'")

    generated_number = np.random.random(300000)
    str_from_number  = ' '.join([str(x) for x in generated_number])
    # print str_from_number
    # print len(str_from_number)

    for xx in range(100):
      response = yield redis2.publish("w00t", "Hello, world!")
      response = yield redis2.publish("w00t",str_from_number)
      response = yield redis2.publish("w00t",str_from_number[4000000:])
      response = yield redis2.publish("w00t",str_from_number[2000000:])

    log.msg("published to w00t, response = %r" % response)

    reactor.stop()


def main():
    log.startLogging(sys.stdout)
    reactor.callLater(0, runTest)
    reactor.run()


if __name__ == "__main__":
	main()
