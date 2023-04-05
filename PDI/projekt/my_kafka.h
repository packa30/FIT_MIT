// Projekt: Ingetstor metadat z network flow do Apache Kafka
// Autory: xkrajc17,xgrofc00
// Datum: 27.11.2020

#include <librdkafka/rdkafka.h>

void dr_msg_cb (rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque);

void send_msg(rd_kafka_t *rk, char *buffer);

rd_kafka_t *init_kafka(char *brokers);
