// Projekt: Ingetstor metadat z network flow do Apache Kafka
// Autory: xkrajc17,xgrofc00
// Datum: 27.11.2020

#ifndef PDI_FLOW_LIST_H
#define PDI_FLOW_LIST_H

#endif //PDI_FLOW_LIST_H
#include "packet_parser.h"
#include "my_kafka.h"

struct item {
  struct Flow *flow;
  struct item *next;
  struct item *prev;
};

struct List {
  struct item *first;
  struct item *actual;
  struct item *last;
};

struct List *list_init();

void list_insert(struct List *l, struct Flow *flow);

int list_find_and_remove(struct List *l, struct flow_id *id);

int list_find_item(struct List *l, struct flow_id *id);

int list_remove(struct List *l, struct flow_id *id, rd_kafka_t *rk);

void dispose_list(struct List *l, rd_kafka_t *rk);

void clear_timers(struct List *l, const int timer, rd_kafka_t *rk);
