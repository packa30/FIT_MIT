#!/usr/bin/env python3

import sys
from kafka import KafkaConsumer

if len(sys.argv) == 3 and sys.argv[1].find(':'):
    server = sys.argv[1]
    topic = sys.argv[2]
else :
    print('Usage python3 consumer.py <server:port> <topic>')
    sys.exit()

group_id = 'group1'

print(sys.argv[1])

print('Connecting to Kafka')
consumer = KafkaConsumer(topic,
                         group_id=group_id,
                         bootstrap_servers=[server],
                         auto_offset_reset='earlieast')
print('Connected to Kafka')

try:
    for message in consumer:
        data = message.value.decode('utf-8',errors="ignore")
        print(data)

except KeyboardInterrupt:
    sys.exit()
