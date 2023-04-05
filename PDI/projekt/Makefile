CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic
CFILES= packet.h flow_list.h flow_list.c packet_parser.h packet_parser.c my_kafka.h my_kafka.c main.c

producer:
	$(CC) $(CFLAGS) $(CFILES) -lm -lrdkafka -o producer

clean:
	rm producer

zip:
	zip  xkrajc17_xgrofc00 *.c *.h *py Makefile script.sh docker-compose.yml
