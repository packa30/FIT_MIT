// Projekt: Ingetstor metadat z network flow do Apache Kafka
// Autory: xkrajc17,xgrofc00
// Datum: 27.11.2020

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "flow_list.h"

volatile int in_List = 0;

struct List *list_init(){
  struct List * l = (struct List *) malloc(sizeof(struct List));

  l->actual = NULL;
  l->last   = NULL;
  l->first  = NULL;
  return l;
}

void list_insert(struct List *l, struct Flow *flow) {
  struct item *new_item = (struct item *)malloc(sizeof(struct item));
  new_item->flow = (struct Flow *)malloc(sizeof(struct Flow));

  strcpy(new_item->flow->id.src,flow->id.src);
  strcpy(new_item->flow->id.dst,flow->id.dst);
  new_item->flow->id.protocol_num = flow->id.protocol_num;
  new_item->flow->id.src_port = flow->id.src_port;
  new_item->flow->id.dst_port = flow->id.dst_port;
  new_item->next = NULL;

  if(l->first == NULL){
    l->first = new_item;
    l->actual = l->first;
    l->last = l->first;
    l->first->prev = NULL;
  }else {
    l->last->next = new_item;
    l->last->next->prev = l->last;
    l->last = l->last->next;
    l->actual = l->last;
  }
}

int list_find_and_remove(struct List *l, struct flow_id *id){
  struct item *actual;
  struct item *prev;

  if(isEqIPv4Flow(id,&l->first->flow->id)){
    actual = l->first;
    l->first = l->first->next;
    free(actual->flow);
    free(actual);
  }else{
    actual = l->first->next;
    prev   = l->first;
    while(actual != NULL){
      if(isEqIPv4Flow(id,&actual->flow->id)){
        prev->next = actual->next;
        free(actual->flow);
        free(actual);
        break;
      }

      prev = actual;
      actual = actual->next;
    }
  }
  l->actual = l->first;
  return 0;
}

int list_find_item(struct List *l, struct flow_id *id) {
  struct item *actual = l->first;
  while (actual != NULL) {
    if(isEqIPv4Flow(id,&actual->flow->id)){
        l->actual = actual;
        return 1;
    }
    actual = actual->next;
  }
  return 0;
}

void clear_timers(struct List *l, const int timer, rd_kafka_t *rk){
  time_t actual_time = time(NULL);
  struct item *tmp;
  char *buffer;
  int duration;
  int cnt = 0;
  while (l->first != NULL) {
     duration = actual_time - get_first_timestamp(l->first->flow);
     if(duration < timer){
       l->actual = l->first;
       break;
     }

     cnt++;
     tmp = l->first;
     l->first = l->first->next;

     if(!tmp->flow->packets.isExported){
       buffer = flow_to_str(tmp->flow);
       send_msg(rk,buffer);
     }

     free(tmp->flow);
     free(tmp);
   }
}

void dispose_list(struct List *l, rd_kafka_t *rk){
  struct item *tmp;
  char *buffer;
  while(l->first != NULL){
    tmp = l->first;
    l->first = l->first->next;

    if(!tmp->flow->packets.isExported){
      buffer = flow_to_str(tmp->flow);
      send_msg(rk,buffer);
    }

    free(tmp->flow);
    free(tmp);
  }
  free(l);
}

int list_remove(struct List *l, struct flow_id *id, rd_kafka_t *rk){
  struct item *tmp;

  if(!isEqIPv4Flow(id,&l->actual->flow->id)){
    return 0;
  }

  if(!l->actual->flow->packets.isExported){
    char * buffer = flow_to_str(l->actual->flow);
    send_msg(rk,buffer);
  }

  if(isEqIPv4Flow(id,&l->first->flow->id)){
    tmp = l->first;
    l->first = l->first->next;
    l->first->prev = NULL;

    free(tmp->flow);
    free(tmp);
  }else{
    tmp = l->actual;

    if(l->actual->next == NULL) {
        l->actual->prev->next = NULL;
    }else{
        l->actual->next->prev = l->actual->prev;
        l->actual->prev->next = l->actual->next;
        l->actual = l->actual->prev;
    }

    free(tmp->flow);
    free(tmp);
  }
  return 0;
}
