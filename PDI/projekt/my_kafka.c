// Projekt: Ingetstor metadat z network flow do Apache Kafka
// Autory: xkrajc17,xgrofc00
// Datum: 27.11.2020

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "my_kafka.h"
#include "flow_list.h"
#define TOPIC "flow"

void dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque) {
  if (rkmessage->err)
    fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
}

void send_msg(rd_kafka_t *rk, char *buffer){
  rd_kafka_resp_err_t error;

  int buffer_len = strlen(buffer) + 1;

  error = rd_kafka_producev(rk, RD_KAFKA_V_TOPIC(TOPIC),
                RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                RD_KAFKA_V_VALUE(buffer, buffer_len),
                RD_KAFKA_V_OPAQUE(NULL),
                RD_KAFKA_V_END);
  if(error){
    fprintf(stderr,"%% Failed to produce to topic %s: %s\n", TOPIC, rd_kafka_err2str(error));
    if (error == RD_KAFKA_RESP_ERR__QUEUE_FULL) {
      rd_kafka_poll(rk, 100);
      send_msg(rk,buffer);
      return;
    }
  }else {
    fprintf(stderr, "%% Enqueued message (%d bytes) for topic %s\n", buffer_len, TOPIC);
  }
  free(buffer);
}

rd_kafka_t *init_kafka(char *brokers){
  rd_kafka_t *rk;         /* Producer instance handle */
  rd_kafka_conf_t *conf;  /* Temporary configuration object */
  char errstr[512];       /* librdkafka API error reporting buffer */

  conf = rd_kafka_conf_new();
  if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
    fprintf(stderr, "%s\n", errstr);
    exit(1);
  }

  rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

  rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));

  if (rk == NULL) {
    fprintf(stderr,"%% Failed to create new producer: %s\n", errstr);
    exit(1);
  }

  return rk;
}
