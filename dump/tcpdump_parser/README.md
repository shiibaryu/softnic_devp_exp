## a parser of text file from tcpdump only for my project

### how to works
19:32:48.408295 IP 192.168.10.1.16384 > omega.16384: NetTLP: MWr, len 2, requester 00:00, tag 0x00, last 0xf, first 0xf, Addr 0x80000020 <br>
↓ <br>
this parser (msec ver.) <br>
↓ <br>
408295,Mwr,2 <br>
↓ <br>
use these parsed data to make a stack graph of traffic by make_stack_graph.py 

