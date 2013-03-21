#!/usr/bin/python
#My second homework for Network.
from PyQt4 import QtCore, QtGui
from ctypes import *
from winpcapy import *
import time
import sys
import string
import platform

#global variable
display_string = ''
ip_packet_flag = 1 	# 1 mean this is ip_packet; 0 mean not 

#*********************************************************************************************************
#for format transform
def ip_to_str(data, offset):
    """ Convert data from an array into a human-readable IP address. """
    return '%d.%d.%d.%d' % tuple(data[offset:offset+4])

def data_to_num(data, offset, len):
    """ Convert data from an array into an integer (big-endian). """
    num = 0
    for i in xrange(len):
        num <<= 8
        num |= data[offset+i]
    return num

#*********************************************************************************************************
if platform.python_version()[0] == "3":
	raw_input=input
#/* prototype of the packet handler */
#void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
PHAND=CFUNCTYPE(None,POINTER(c_ubyte),POINTER(pcap_pkthdr),POINTER(c_ubyte))

#*********************************************************************************************************
## Callback function invoked by libpcap for every incoming packet
def _packet_handler(param, header, pkt_data):

	## convert the timestamp to readable format
	local_tv_sec = header.contents.ts.tv_sec
	ltime=time.localtime(local_tv_sec);
	timestr=time.strftime("%H:%M:%S", ltime)

#XXX:pkt_data's bitmap
# 0-5 dest-mac
# 6-11 sours-mac
# 12-13 ether_type
# 14 ip_version + ip_header_length
# 15 service_type
# 16-17 ip_length
# 18-19 ip_identifier 
# 20-21 0 + DFflags + MFflags + fragment offset
# 22 TTL
# 23 ip_protocol
# 24-25 headerchecksum
# 26-29 source_ip
# 30-33 dest_ip

	global display_string
	global ip_packet_flag
	
	display_string = ''
	
	# if this packet an IP packet or not?
	ether_type = data_to_num(pkt_data, 12, 2)
	if ether_type != 0x0800:
		ip_packet_flag = 0
		print '**************************************'
		print 'Ether_type : ' + str(hex(ether_type)) + '. Not IP-packet!'
		print 
		return
	else:
		ip_packet_flag = 1

	#Ip-header 
	ip_version = (pkt_data[14] >> 4) & 15
	ip_header_length = (pkt_data[14] & 15) * 4
	service_type = pkt_data[15]
	ip_length = data_to_num(pkt_data, 16, 2)
	ip_identifier = data_to_num(pkt_data, 18, 2)
	ip_DFflags = ((pkt_data[20] >> 6) & 1)
	ip_MFflags = ((pkt_data[20] >> 5) & 1)
	ip_fragment_offset = ((pkt_data[20] & 31) << 8) | pkt_data[21]
	ip_TTL = pkt_data[22]
	ip_protocol = pkt_data[23]
	ip_headerchecksum = data_to_num(pkt_data, 24, 2)
	source_ip = ip_to_str(pkt_data, 26)
	dest_ip = ip_to_str(pkt_data, 30)

	print '**************************************'
	if ip_version == 4:
		print 'ip_version : IPv4'
		display_string = display_string + 'ip_version : IPv4' + '\n'
	elif ip_version == 6:
		print 'ip_version : IPv6'
		display_string = display_string + 'ip_version : IPv6' + '\n'
	print 'ip_header_length : ' + str(ip_header_length)
	display_string = display_string + 'ip_header_length : ' + str(ip_header_length) + '\n'
	print 'service_type : ' + str(service_type)
	display_string = display_string + 'service_type : ' + str(service_type) + '\n'
	print 'ip_length : ' + str(ip_length)
	display_string = display_string + 'ip_length : ' + str(ip_length) + '\n'
	print 'ip_identifier : ' + str(ip_identifier)
	display_string = display_string + 'ip_identifier : ' + str(ip_identifier) + '\n'
	print 'ip_DFflags : ' + str(ip_DFflags)
	display_string = display_string + 'ip_DFflags : ' + str(ip_DFflags) + '\n'
	print 'ip_MFflags : ' + str(ip_MFflags)
	display_string = display_string + 'ip_MFflags : ' + str(ip_MFflags) + '\n'
	print 'ip_fragment_offset : ' + str(ip_fragment_offset)
	display_string = display_string + 'ip_fragment_offset : ' + str(ip_fragment_offset) + '\n'
	print 'ip_TTL : ' + str(ip_TTL)
	display_string = display_string + 'ip_TTL : ' + str(ip_TTL) + '\n'
	if ip_protocol == 6:
		print 'ip_protocol : TCP'
		display_string = display_string + 'ip_protocol : TCP' + '\n'
	elif ip_protocol == 17:
		print 'ip_protocol : UDP'
		display_string = display_string + 'ip_protocol : UDP' + '\n'
	elif ip_protocol == 1:
		print 'ip_protocol : ICMP'
		display_string = display_string + 'ip_protocol : ICMP' + '\n'
	elif ip_protocol == 2:
		print 'ip_protocol : IGMP'
		display_string = display_string + 'ip_protocol : IGMP' + '\n'
	elif ip_protocol == 89:
		print 'ip_protocol : OSPF'
		display_string = display_string + 'ip_protocol : OSPF' + '\n'
	else:
		print 'ip_protocol : ' + str(ip_protocol)
		display_string = display_string + 'ip_protocol : ' + str(ip_protocol) + '\n'
	print 'ip_headerchecksum : ' + str(ip_headerchecksum)
	display_string = display_string + 'ip_headerchecksum : ' + str(ip_headerchecksum) + '\n'
	print 'source_ip : '+ str(source_ip)
	display_string = display_string + 'source_ip : '+ str(source_ip) + '\n'
	print 'dest_ip : ' + str(dest_ip)
	display_string = display_string + 'dest_ip : ' + str(dest_ip) + '\n'

	print("%s,%.6d len:%d" % (timestr, header.contents.ts.tv_usec, header.contents.len))

#*********************************************************************************************************

packet_handler=PHAND(_packet_handler)
alldevs=POINTER(pcap_if_t)()
errbuf= create_string_buffer(PCAP_ERRBUF_SIZE)
## Retrieve the device list
if (pcap_findalldevs(byref(alldevs), errbuf) == -1):
	print ("Error in pcap_findalldevs: %s\n" % errbuf.value)
	sys.exit(1)

###############################################################################
if 0: 
	## Print the list
	i=0
	try:
		d=alldevs.contents
	except:
		print ("Error in pcap_findalldevs: %s" % errbuf.value)
		print ("Maybe you need admin privilege?\n")
		sys.exit(1)
	while d:
		i=i+1
		print("%d. %s" % (i, d.name))
		if (d.description):
			print (" (%s)\n" % (d.description))
		else:
			print (" (No description available)\n")
		if d.next:
			d=d.next.contents
		else:
			d=False

	if (i==0):
		print ("\nNo interfaces found! Make sure WinPcap is installed.\n")
		sys.exit(-1)



	print ("Enter the interface number (1-%d):" % (i))
	inum= raw_input('--> ')
	if inum in string.digits:
		inum=int(inum)
	else:
		inum=0
	if ((inum < 1) | (inum > i)):
		print ("\nInterface number out of range.\n")
		## Free the device list
		pcap_freealldevs(alldevs)
		sys.exit(-1)
###############################################################################

inum = 1 #for my own computer,i have known the NIC number
## Jump to the selected adapter
d=alldevs
for i in range(0,inum-1):
	d=d.contents.next

## Open the device
## Open the adapter
d=d.contents

adhandle = pcap_open_live(d.name,65536,1,1000,errbuf)
#65536 means capture the entire packet
#

if (adhandle == None):
	print("\nUnable to open the adapter. %s is not supported by Pcap-WinPcap\n" % d.contents.name)
	## Free the device list
	pcap_freealldevs(alldevs)
	sys.exit(-1)
	
print("\nlistening on %s...\n" % (d.description))

## At this point, we don't need any more the device list. Free it
pcap_freealldevs(alldevs)

## start the capture
#pcap_loop(adhandle, capture_number, packet_handler, None)
#pcap_close(adhandle)


#***********************************************************************************************************
#GUI
#action for button'capture'
def capture():
	capture_number = int(str(inputEdit.text()))
	if capture_number < 0:
		info.setText('Error Input!...')
		return
	info.setText('Capture list following...')
	n_tmp = 0
	while True:
		if  n_tmp == capture_number :
			break
		pcap_loop(adhandle, 1, packet_handler, None)	## start the capture
		if ip_packet_flag == 1:
			n_tmp = n_tmp + 1
			info.append('Packet ' + str(n_tmp) + ' :')
			info.append(display_string)
			info.update()
	#pcap_close(adhandle)
	

app = QtGui.QApplication(sys.argv)

#Create the widget
widget = QtGui.QWidget()
widget.setWindowTitle('Packet Capture by Liulu-2120120370')
widget.setGeometry(300, 300, 700, 500)


#Create the component
b1 = QtGui.QPushButton('Capture', widget)
widget.connect(b1 ,QtCore.SIGNAL('clicked()'), capture)
inputNum = QtGui.QLabel('Capture Number : ',widget)
inputEdit = QtGui.QLineEdit('5')
info = QtGui.QTextBrowser(widget)
info.setText('Please input the number of packet to capture.')


#Set the Layout
grid = QtGui.QGridLayout()
grid.addWidget(inputNum,1,0)
grid.addWidget(inputEdit,1,1)
grid.addWidget(b1,2,0)
grid.addWidget(info,3,0,4,2)
widget.setLayout(grid)

widget.show()
sys.exit(app.exec_())
