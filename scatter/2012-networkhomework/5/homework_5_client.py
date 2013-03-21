##################################################
#The Fifth Network-homework
#Chat
#2012.12.20
##################################################
#lib-import
import os, sys, string
from PyQt4 import QtCore, QtGui
from ctypes import *
import platform
import socket, time, thread , threading

local_ip = '12.12.12.67'
global online_user
online_user = {}
##################################################
def login():
	s=socket.socket()
	s.settimeout(2)
	s.connect(('12.12.12.114',6000))
	while True:
                try:
                        username_l = username.text()
                        password_l = password.text()
                        s.send(str(username_l))
                        s.send(str(password_l))
                        answer = s.recv(512)
                except:
                        continue
                
		if answer == 'Welcome back!':
			print answer
			break
		else:
			online_list.setText(answer)
			online_list.update()
			print answer	

	# print login list
	login_list = s.recv(1024)
	online_list.setText('')
	online_list.append('User-online: \n')
	online_list.append(login_list)
	online_list.update()

	b1.setEnabled(1)
	b2.setEnabled(0)

	th_bei = threading.Thread(target = beidong , args= [])
        th_bei.start()

	s.close()

###############################################################
def clicksend():
        th_zhu = threading.Thread(target = zhudong , args= [])
        th_zhu.start()

###############################################################
def zhudong():
	global glock
	#choose whom to chat
	#glock.acquire()
	#ask_ip = raw_input('His ip > ')
	#ask_port = raw_input('His port > ')
	#glock.release()
	address = (str(talkto.text()), 70000)
	s_ask = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        s_message = send_data.toPlainText()     #XXX:
        s_ask.sendto(str(s_message), address)
        
        show_data.append('Me: ' + s_message +'\n')
        show_data.update()


        s_ask.close()
    
########################################################
def beidong():
        global glock
        s_recv = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s_recv.bind((local_ip, 70000))

        #glock.acquire()
        while True:
                data,(addr, port) = s_recv.recvfrom(1024)
                print addr, ' : ', data
                show_data.append(str(addr) + ' : ' + data + '\n')
                show_data.update()

        #glock.release()

#########################################################################################

#GUI For display
app = QtGui.QApplication(sys.argv)
#Create the widget
widget = QtGui.QWidget()
widget.setWindowTitle('Chat by Liu Lu 2120120370')
widget.setGeometry(300, 300, 600, 500)

#Create the component
b1 = QtGui.QPushButton('Send', widget)
b2 = QtGui.QPushButton('Login', widget)
b3 = QtGui.QPushButton('Quit', widget)
b1.setEnabled(0)

#Create the response-connection
widget.connect(b1 ,QtCore.SIGNAL('clicked()'), clicksend)
widget.connect(b2 ,QtCore.SIGNAL('clicked()'), login)


name_label = QtGui.QLabel('Username : ',widget)
password_label = QtGui.QLabel('Username : ',widget)
#online_label = QtGui.QLabel('Online-User : ',widget)

online_list = QtGui.QTextBrowser(widget)
online_list.setText('')

username = QtGui.QLineEdit('liulu')
password = QtGui.QLineEdit('123')
password.setEchoMode(QtGui.QLineEdit.Password)
talkto_label = QtGui.QLabel('Talk to :')
talkto = QtGui.QLineEdit('')
talkto.setText('*.*.*.*')

send_data = QtGui.QTextEdit('', widget)

show_data = QtGui.QTextBrowser(widget)
show_data.setText('')

#Set the Layout
grid = QtGui.QGridLayout()
grid.addWidget(name_label,1,1,1,1)
grid.addWidget(username,1,2,1,1)

grid.addWidget(password_label,2,1,1,1)
grid.addWidget(password,2,2,1,1)
	
grid.addWidget(b2,3,1,1,2)

#grid.addWidget(online_label,4,1,1,2)
	
grid.addWidget(online_list,4,1,3,2)
grid.addWidget(b3,7,1,1,1)
grid.addWidget(b1,7,2,1,1)

grid.addWidget(show_data,1,3,4,3)
grid.addWidget(talkto_label,5,3,1,1)
grid.addWidget(talkto,5,4,1,2)
grid.addWidget(send_data,6,3,2,3)

widget.setLayout(grid)
widget.show()

#global glock
#glock = threading.Lock()

sys.exit(app.exec_())
