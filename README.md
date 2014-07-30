v800_downloader
===============

Tool to download data files from the Polar V800 GPS watch

Communication Format:
The V800 uses interrupt transfers to communicate with the PC; 64 byte packets are passed from PC->V800 and V800->PC. Generally the PC will initiate a transfer of data and the V800 will reply. Each reply from the V800 must be followed by an ACK type packet from the PC to get the next packet from the V800. In this way you can ask to get a ROUTE.GZB file that is spanned across hundreds of packets by having a very fast request->data->ack->data->ack....->last data packet.

The data packets from the V800 contain a length that is comprised of one byte. The byte is split into two parts, bits 0-1 indicating if there is a follow on packet (1 = follow on packet, 0 = no follow on packet) and bits 2-7 indicating the size of the packet. The first two bits allow you to see if the packet is the last packet for the request. Oddly the first packet from the V800 for every request contains 2 extra 0x00 at the start so they must be dropped in order to make valid files.
