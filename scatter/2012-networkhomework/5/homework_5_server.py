#!/usr/bin/env python 
import SocketServer ,time
from time import ctime 
HOST = '12.12.12.114' 
PORT = 6000 
ADDR = (HOST, PORT) 

connect_list = []
login_list = {}
userinfo = {'l':'123','peng':'456','xiao':'789'}

class MyRequestHandler(SocketServer.BaseRequestHandler): 
    def handle(self): 
        print '...connected from:', self.client_address 
        connect_list.append(self.client_address)
        
	#indentity
	while True: 	
		username = self.request.recv(512)
		password = self.request.recv(512)
		if not userinfo.has_key(username):
			self.request.sendall('Username not exists!')
		else:
			if userinfo.get(username) != password:
				self.request.sendall('Wrong password!') 
			else:
				self.request.sendall('Welcome back!')
				print username,' logins.'
				
				login_list[username] = self.client_address[0]
				
				break


	#send login_list
	self.request.sendall(str(login_list))

        #receive data from the client
        while True:
        	try:
        		data = self.request.recv(1024)
        		if not data:
        			continue
        		self.request.sendall('[%s] : %s' % (ctime(), data)) 
        		print self.client_address , ' : ',data
		except:
			print 'Error!'
			break
			
	print self.client_address , ': quit! '
	connect_list.remove(self.client_address)
	login_list.pop(username)


tcpServ = SocketServer.ThreadingTCPServer(ADDR, MyRequestHandler) 
print 'waiting for connection...' 
tcpServ.serve_forever()
