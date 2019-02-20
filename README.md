# clientForkServTCP6
Simple Client (with DNS resolution) / ForkServer TCPv6

ps: The best compatibility practice is to define ai_family (client and server) always as AF_UNSPEC (for getaddrinfo() and socket()).
