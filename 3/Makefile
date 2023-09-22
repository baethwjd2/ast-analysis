all:
	gcc -o spoof_tcp  spoof_tcp.c  checksum.c send_raw_ip_packet.c
	gcc -o sniff_improved sniff_improved.c -lpcap

clean:
	rm -f spoof_tcp *.o *.out
	rm -f sniff_improved