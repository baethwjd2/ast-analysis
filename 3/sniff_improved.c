#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>

#include "myheadr.h"

void got_packet(u_char *args, const struct pcap_pkthdr *header,
                              const u_char *packet)
{
  struct ethheader *eth = (struct ethheader *)packet;

  // 출력 대상 MAC 주소를 문자열로 변환
  char dest_mac[18];
  char source_mac[18];
  printf("\n");
  sprintf(dest_mac, "%02x:%02x:%02x:%02x:%02x:%02x", eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2], eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);
  sprintf(source_mac, "%02x:%02x:%02x:%02x:%02x:%02x", eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2], eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);

  printf("Destination MAC: %s\n", dest_mac);
  printf("     Source MAC: %s\n", source_mac);

  if (ntohs(eth->ether_type) == 0x0800) { // 0x0800 is IP type
    struct ipheader * ip = (struct ipheader *)
                           (packet + sizeof(struct ethheader)); 

    printf("Destination IP: %s\n", inet_ntoa(ip->iph_destip));    
    printf("     Source IP: %s\n", inet_ntoa(ip->iph_sourceip));   

    /* determine protocol */
    switch(ip->iph_protocol) {                                 
        case IPPROTO_TCP:
            printf("   Protocol: TCP\n");
            struct tcpheader *tcp = (struct tcpheader *)(packet + sizeof(struct ethheader) + sizeof(struct ipheader));

            printf("   Destination Port: %d\n", ntohs(tcp->tcp_dport));
            printf("        Source Port: %d\n", ntohs(tcp->tcp_sport));
            // TCP 데이터의 위치 계산
            int tcp_data_offset = sizeof(struct ethheader) + sizeof(struct ipheader) + (TH_OFF(tcp) * 4);

            // TCP 데이터 길이 계산
            int tcp_data_length = ntohs(ip->iph_len) - (sizeof(struct ipheader) + (TH_OFF(tcp) * 4));
    
            // TCP 데이터를 출력
            printf("           TCP DATA: ");
            for (int i = 0; i < tcp_data_length; i++) {
              printf("%c", packet[tcp_data_offset + i]);
            }
            printf("\n");
            return;
        case IPPROTO_UDP:
            printf("   Protocol: UDP\n");
            return;
        case IPPROTO_ICMP:
            printf("   Protocol: ICMP\n");
            return;
        default:
            printf("   Protocol: others\n");
            return;
    }
  }
}

int main()
{
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[] = "icmp or tcp";
  bpf_u_int32 net;

  // Step 1: Open live pcap session on NIC with name enp0s3
  handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);

  // Step 2: Compile filter_exp into BPF psuedo-code
  pcap_compile(handle, &fp, filter_exp, 0, net);
  if (pcap_setfilter(handle, &fp) !=0) {
      pcap_perror(handle, "Error:");
      exit(EXIT_FAILURE);
  }

  // Step 3: Capture packets
  pcap_loop(handle, -1, got_packet, NULL);

  pcap_close(handle);   //Close the handle
  return 0;
}

