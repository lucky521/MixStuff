##################################################
#The Fourth Network-homework
#E-mail-POP3
#Liu Lu - 2120120370
#2012.12.10
##################################################
#lib-import
import os, sys, string
import poplib, email
from PyQt4 import QtCore, QtGui
from ctypes import *
import platform
#################################################
def showmessage(mail):
	if mail.is_multipart():
		for part in mail.get_payload():
			showmessage(part)
	else:
		return mail.get_payload()
###########################################################################################
def recv():
	pp = poplib.POP3(str(host.text()))
	pp.set_debuglevel(1)
	pp.user(str(username.text()))	#XXX:USER
	pp.pass_(str(password.text()))	#XXX:PASS

	try:
		ret = pp.stat()		#XXX:STAT
		mail_num.setText('MailNum: '+str(ret[0])+ ' mails.')

		b2.setEnabled(1)
		pp.quit()
	except:
		print
		print 'Error!'
		pp.quit()

def read():
	pp = poplib.POP3(str(host.text()))
	pp.set_debuglevel(1)
	pp.user(str(username.text()))	#XXX:USER
	pp.pass_(str(password.text()))	#XXX:PASS
	ret = pp.stat()	

	if int(read_num.text()) <= ret[0]:
		hdr,text,octet = pp.retr(int(read_num.text()))	#XXX:RETR
		mail = email.message_from_string(string.join(text,'\n'))
		mail_info.setText('')
		mail_info.append('To: ' + mail['To'])
		mail_info.append('From: ' + mail['From'])
		mail_info.append('Date: ' + mail['Date'])
		mail_info.append('Subject: ' + mail['subject'])
		mail_info.append('Content: ' + showmessage(mail))
		mail_info.update()
	else:
		mail_info.setText('Not exists.')
		mail_info.update()
		

	pp.quit()
#########################################################################################
#GUI For display
app = QtGui.QApplication(sys.argv)
#Create the widget
widget = QtGui.QWidget()
widget.setWindowTitle('Email-POP3 by Liu Lu 2120120370')
widget.setGeometry(300, 300, 600, 500)

#Create the component
b1 = QtGui.QPushButton('Receive', widget)
b2 = QtGui.QPushButton('Read', widget)
b2.setEnabled(0)

#Create the response-connection
widget.connect(b1 ,QtCore.SIGNAL('clicked()'), recv)
widget.connect(b2 ,QtCore.SIGNAL('clicked()'), read)

host_label = QtGui.QLabel('MailServerName : ',widget)
host = QtGui.QLineEdit('pop3.***.com')

username_label = QtGui.QLabel('UserName : ',widget)
username = QtGui.QLineEdit('***@***.com')

password_label = QtGui.QLabel('Password : ',widget)
password = QtGui.QLineEdit('')



mail_num = QtGui.QLabel(widget)
mail_num.setText('MailNum: ')

r_num = QtGui.QLabel(widget)
r_num.setText('Read Number: ')

read_num = QtGui.QLineEdit(widget)
read_num.setText('')

mail_info = QtGui.QTextBrowser(widget)
mail_info.setText('')

#Set the Layout
grid = QtGui.QGridLayout()
grid.addWidget(host_label,1,0,1,1)
grid.addWidget(host,1,1,1,1)
grid.addWidget(b1,1,3,1,1)
grid.addWidget(username_label,2,0,1,1)
grid.addWidget(username,2,1,1,1)
grid.addWidget(password_label,3,0,1,1)
grid.addWidget(password,3,1,1,1)
grid.addWidget(r_num,4,0,1,1)
grid.addWidget(read_num,4,1,1,1)
grid.addWidget(mail_num,4,2,1,1)
grid.addWidget(b2,2,3,1,1)

grid.addWidget(mail_info,5,0,1,4)

widget.setLayout(grid)

widget.show()
sys.exit(app.exec_())
########################################################################################
