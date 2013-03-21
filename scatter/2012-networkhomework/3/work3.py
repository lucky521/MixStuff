##################################################
#The Third Network-homework
#Tcp and Udp scanner
##################################################
#lib-import
import threading , time , Queue , socket ,struct
from PyQt4 import QtCore, QtGui
from ctypes import *
import time
import sys
import string
import platform
#################################################
#global struct
target = []
queue = Queue.Queue()
queue_udp = Queue.Queue()
###########################################################################################
def udp_sender(ip,port):
    try:
        ADDR = (ip,port)
        sock_udp = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)	#Create UDP socket
        sock_udp.sendto("",ADDR)		#Send a empty udp-packet
        sock_udp.close()
    except:
        pass


def icmp_receiver(ip,port):
    icmp = socket.getprotobyname("icmp")	
    try:
        sock_icmp = socket.socket(socket.AF_INET, socket.SOCK_RAW, icmp)
    except socket.error, (errno, msg):
        if errno == 1:
            # Operation not permitted, I have to get the root privilege
            msg = msg + (
                " - Note that ICMP messages can only be sent from processes"
                " running as root."
            )
            raise socket.error(msg)
        raise 	# raise the original error
    sock_icmp.settimeout(3)
    try:
        recPacket,addr = sock_icmp.recvfrom(64)
    except:
        queue_udp.put(True)
        return
    icmpHeader = recPacket[20:28]
    icmpPort = int(recPacket.encode('hex')[100:104],16)
    head_type, code, checksum, packetID, sequence = struct.unpack("bbHHh", icmpHeader)
    sock_icmp.close()
    if code == 3 and icmpPort == port and addr[0] == ip :
        queue_udp.put(False)
    return



def checker_udp(ip,port):
    thread_udp = threading.Thread(target=udp_sender,args=(ip,port))
    thread_icmp = threading.Thread(target=icmp_receiver,args=(ip,port))
    
    thread_udp.daemon= True
    thread_icmp.daemon = True
    
    thread_icmp.start()
    time.sleep(0.1)
    thread_udp.start()
    
    thread_icmp.join()
    thread_udp.join()
    return queue_udp.get(False)
###########################################################################################
def udp_scan():
	port_from_num = int(str(port_from.text()))
	port_to_num = int(str(port_to.text()))
	hostaddress_num = str(hostaddress.text())
	port = [ i for i in range(port_from_num, port_to_num + 1) ]
	
	target.append(hostaddress_num)
	for i in target:
	    queue.put(i)
	   
	class ScanThread(threading.Thread):
	    def run(self):
		global queue
		ip = queue.get()
		for p in port:
			if checker_udp(ip, int(p)):
				display_string = "IP:%s     UDP-Port:%d     Open" %( ip , int(p) )
				info.append(display_string)
				info.update()
				print '========================================================='
				print "IP:%s    , Port:%d    , Open" %( ip , int(p) )
			else:
			    	display_string = "IP:%s     UDP-Port:%d     Close" %( ip , int(p) )
				info.append(display_string)
				info.update()
			
		    
		queue.task_done()

		
	for p in range(len(target)):
	    ScanThread().start()
	print "waiting for thread ..."
	queue.join()

###########################################################################################
def tcp_scan():
	port_from_num = int(str(port_from.text()))
	port_to_num = int(str(port_to.text()))
	hostaddress_num = str(hostaddress.text())

	port = [ i for i in range(port_from_num, port_to_num + 1) ]
	
	target.append(hostaddress_num)
	for i in target:
	    queue.put(i)
	    
	class ScanThread(threading.Thread):
	    def run(self):
		global queue
		ip = queue.get()
		for p in port:
		    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)	#Create the socket
		    s.settimeout(3) 	#set the Timeout
		    try:
		        s.connect((ip,int(p)))		#connect 
		        display_string = "IP:%s     TCP-Port:%d     Open" %( ip , int(p) )
		        info.append(display_string)
		        info.update()
		        print '========================================================='
		        print "IP:%s    , Port:%d    , Open" %( ip , int(p) )
		    except:
		    	display_string = "IP:%s     TCP-Port:%d     Close" %( ip , int(p) )
			info.append(display_string)
			info.update()
		    s.close()
		    
		queue.task_done()

		
	for p in range(len(target)):
	    ScanThread().start()
	print "waiting for thread ..."
	queue.join()

#########################################################################################
#GUI For display
app = QtGui.QApplication(sys.argv)
#Create the widget
widget = QtGui.QWidget()
widget.setWindowTitle('Port Scanner by Liu Lu 2120120370')
widget.setGeometry(300, 300, 500, 300)

#Create the component
b1 = QtGui.QPushButton('TCP_Scan', widget)
b2 = QtGui.QPushButton('UDP_Scan', widget)

#Create the response-connection
widget.connect(b1 ,QtCore.SIGNAL('clicked()'), tcp_scan)
widget.connect(b2 ,QtCore.SIGNAL('clicked()'), udp_scan)


address_label = QtGui.QLabel('Host Address : ',widget)
hostaddress = QtGui.QLineEdit('12.12.12.67')

port_label = QtGui.QLabel('Port Number : ',widget)
port_from = QtGui.QLineEdit('')
port_to = QtGui.QLineEdit('')

info = QtGui.QTextBrowser(widget)
info.setText('')

#Set the Layout
grid = QtGui.QGridLayout()
grid.addWidget(address_label,1,0,1,1)
grid.addWidget(hostaddress,1,1,1,1)
grid.addWidget(port_label,2,0,1,1)
grid.addWidget(port_from,2,1,1,1)
grid.addWidget(port_to,2,2,1,1)
grid.addWidget(info,3,0,1,3)
grid.addWidget(b1,4,1,1,1)
grid.addWidget(b2,4,2,1,1)
widget.setLayout(grid)

widget.show()
sys.exit(app.exec_())
########################################################################################
