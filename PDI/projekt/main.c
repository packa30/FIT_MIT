// Projekt: Ingetstor metadat z network flow do Apache Kafka
// Autory: xkrajc17,xgrofc00
// Datum: 27.11.2020

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <time.h>

#include "packet.h"
#include "flow_list.h"
#include "my_kafka.h"

volatile int run = 1;

void sighandler() {
  run = 0;
}

int main(int argc, char **argv) {
  if(argc != 4){
    fprintf(stderr, "Usage: ./producer <server> <interface> <timer>\n");
    return 1;
  }

  struct sockaddr saddr;
  struct t_TCP_header *tcp_header;
  struct t_IPv4_header *ip_header;
  struct t_IPv6_header *ipv6_header;
  struct flow_id *id;
  struct List *list;
  rd_kafka_t *rk ;

  list = list_init();
  rk = init_kafka(argv[1]);

  const int TIMER = atoi(argv[3]);

  time_t fd, actual_time, last_clear_time, saddr_size;

  last_clear_time  = time(NULL);
  saddr_size = sizeof(saddr);

  char *msg;
  char *buffer = (char *) malloc(65536);

  if (signal(SIGINT, &sighandler) == SIG_ERR) {
    fprintf(stderr, "Could not set signal handler\n");
    return 1;
  }

  if ((fd = socket(AF_PACKET,SOCK_RAW, htons(ETH_P_ALL))) < 0) {
    perror("Can not create RAW socket");
    exit(1);
  }

  if(strcmp(argv[2],"0.0.0.0") != 0){
    setsockopt(fd , SOL_SOCKET , SO_BINDTODEVICE , argv[2] , strlen(argv[2])+ 1 );
  }

  while(run) {
    actual_time = time(NULL);

    if(actual_time - last_clear_time > TIMER){
      last_clear_time = actual_time;
      clear_timers(list,TIMER,rk);
    }

    if((recvfrom(fd, buffer, 65536, 0, &saddr, (socklen_t *) &saddr_size)) < 0){
      continue;
    }

    ip_header = (struct t_IPv4_header*)(buffer + ETHER_HEADER_LEN);

    if ( is_ipv4(ip_header)) {
      if(ip_header->ip_protocol == TCP_PROT_NUM || ip_header->ip_protocol == UDP_PROT_NUM){
        int prefix = ipv4_header_prefix((unsigned int)ip_header);
        id = create_flow_id((const char*)buffer,prefix);

        if(!list_find_item(list,id)){
          list_insert(list,init_flow(id));
        }

        create_packet_info(list->actual->flow,(const char *)buffer,prefix,id->protocol_num);

        if(list->actual->flow->packets.index == 20 && list->actual->flow->packets.isExported == 0){
          list->actual->flow->packets.isExported = 1;
          msg = flow_to_str(list->actual->flow);
          send_msg(rk,msg);
          continue;
        }

        tcp_header = (struct t_TCP_header*)(buffer + ETHER_HEADER_LEN + prefix);
        if(id->protocol_num == TCP_PROT_NUM && tcp_header->flags & 0x01){
          list_remove(list,id,rk);
        }

        ip_header = NULL;
        id = NULL;
      }
    }else if(is_ipv6(ip_header)){
      ipv6_header = (struct t_IPv6_header*)(buffer + ETHER_HEADER_LEN);
      if(ipv6_header->next_header == TCP_PROT_NUM ||  ipv6_header->next_header == UDP_PROT_NUM){
        id = create_flow_id_v6((const char*)buffer,IPV6_HEADER_LEN);

        if(!list_find_item(list,id)){
          list_insert(list,init_flow(id));
        }

        create_packet_info_v6(list->actual->flow,(const char *)buffer,IPV6_HEADER_LEN,id->protocol_num);

        if(list->actual->flow->packets.index == 20 && list->actual->flow->packets.isExported == 0){
          list->actual->flow->packets.isExported = 1;
          msg = flow_to_str(list->actual->flow);
          send_msg(rk,msg);
          continue;
        }

        tcp_header = (struct t_TCP_header*)(buffer + ETHER_HEADER_LEN + IPV6_HEADER_LEN);

        if(id->protocol_num == TCP_PROT_NUM && tcp_header->flags & 0x01){
          list_remove(list,id,rk);
        }

        ipv6_header = NULL;
        id = NULL;
      }
    }
  }

  free(buffer);
  dispose_list(list,rk);

  rd_kafka_flush(rk,100);
    if (rd_kafka_outq_len(rk) > 0)
    rd_kafka_outq_len(rk);
  rd_kafka_destroy(rk);
  close(fd);
  return 0;
}
