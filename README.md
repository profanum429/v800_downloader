v800_downloader
===============

Tool to download data files from the Polar V800 GPS watch

Communication Format:
The V800 uses interrupt transfers to communicate with the PC; 64 byte packets are passed from PC->V800 and V800->PC. Generally the PC will initiate a transfer of data and the V800 will reply. Each reply from the V800 must be followed by an ACK type packet from the PC to get the next packet from the V800. In this way you can ask to get a ROUTE.GZB file that is spanned across hundreds of packets by having a very fast request->data->ack->data->ack....->last data packet.

The data packets from the V800 contain a length that is comprised of one byte. The byte is split into two parts, bits 0-1 indicating if there is a follow on packet (1 = follow on packet, 0 = no follow on packet) and bits 2-7 indicating the size of the packet. The first two bits allow you to see if the packet is the last packet for the request. Oddly the first packet from the V800 for every request contains 2 extra 0x00 at the start so they must be dropped in order to make valid files.

The V800 has a filesystem that is laid out like /U/0/ with various directories and files underneath it. This might hint at the possibility of multiple users in the future with a /U/1/, /U/2,... etc. Under the /U/0 directory is a few files like USERID.BPB and a bunch of date directories. These follow the format of yyyyMMdd, eg) 20140729 for July 29th, 2014. Under the date directories are an ACT directory that contains activity info (steps, slices, that sort of thing) and an E directory that contains Exercise Sessions. Under the E directory are time directories in the format of HHMMSS, eg) 104034. Under these time directories are a session info file TSESS.BPB and a physical data file PHYSDATA.BPB and a 00/ directory. I assume 01/, 02/,... etc. are valid and might exist as some point, possibly on a multisport session but I have not tested this yet. Under the 00/ directory is the actual session info; ROUTE.GZB and SAMPLES.GZB being the big two files with the GPS info in the ROUTE file and HR/Cadence/Sensor data in the SAMPLES file. Both are gzipped.

To request the contents of directory or a file the following data is sent:
0x01 ((length of the requested dir/file+8) << 2) 0x00 (length of requested dir/file+4) 0x00 0x08 0x00 0x12 (length of requested dir/file) (requested dir/file)

When recieving a packet it follows this format:
Data[1] & 0x03 == 1 then this packet is not the last packet in the transmission
Data[1] & 0x03 == 0 then this packet is the last packet in the transmission
The length of the packet is data[1] >> 2
Data[2] is an echo of the packet number, starting at 0x00
If this is the first packet, we skip the first 5 bytes of the data for saving purposes. If it is not the first packet we skip the first three bytes of the packet. The rest of the packet is data from the V800 without headers.

When sending an ACK it follows this format:
0x01 0x05 (packet number)
Packet number is a byte between 0x00 and 0xFF. When it reaches 0xFF this will just roll over back to 0x00.

When recieving a listing of a directory the data contains a marker for where the directory/file names are. Inside of the data will be groupings of 0x0A (some byte) 0x0A (length) (string of length). These are the directories and files inside of the requested directory. Extracting them is straightforward at this point and they can be saved to dive further into the V800.

Currently I have no data on how to push/delete/rename files on the V800 and don't plan on introducing this functionality but it would probably be pretty straightfoward to do using Wireshark+USBpcap on Windows if someone desires to do that.
