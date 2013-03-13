#coding=utf-8  
import dpkt  
import sendpkt  
from socket import inet_aton  
from time import strftime  
import socket  
import types  
import uuid
##################################################################
def get_local_mac():  
    mac=uuid.uuid1().hex[-12:]  
    return '-'.join([mac[(i-1)*2:2*i] for i in range(1,7)])  


def trans(ip,mask='255.255.255.0'):  
    ''''' 
          判断两个ip地址是否在同一个网段
    '''  
    str=[]  
    ip=ip.split(".")  	#ip[]
    mask=mask.split(".")	#mask[]  
    for index,item in enumerate(ip):  
        str.append( int(item)&int(mask[index]) )  
    return str
  

def pack_mac(mac,pattern='-'):  
    ''''' 
         网卡地址转为以太网Mac地址 
         例如将"08-00-27-ba-f7-e5"转为"\x08\x00'\xba\xf7\xe5"
    '''
    mac=mac.split(pattern.lower())
    return "".join([chr(int('0x'+x,16)) for x in mac])
#################################################################
def send_msg(kwargs): 
    if type(kwargs) is not types.DictType:   #这个格式判断可以学习一下。虽然在这个程序里没什么用。
        return  
    #源ip地址，如果字典里没添，就获取真实地址 
    src_ip=inet_aton(kwargs.get('src', socket.gethostbyname(socket.gethostname())))  
    #目的ip地址  
    dst_ip=inet_aton(kwargs.get('dst'))
    #源mac地址，如果字典里没添，就获取真实mac地址
    src_mac=pack_mac(kwargs.get('src_mac',get_local_mac()))
    #目的mac地址  
    dst_mac=pack_mac(kwargs.get('dst_mac')) 
    
    host=kwargs.get('host',socket.gethostname())  
    user=kwargs.get('user')  
    msg=kwargs.get('msg')  


    #288表示发送信息
    #msg="1_lbt2_2#0#002564CA9C71#0#0#0:%s:%s:%s:288:%s" %(int(strftime('%m%d%H%M%S'))+100000000, user, host, msg)   #应用层的数据
    msg="1_lbt2_2#0#002564CA9C71#0#0#0:%s:%s:%s:209:%s" %(int(strftime('%m%d%H%M%S'))+100000000, user, host, msg)   #应用层的数据
    
    msg=msg.decode('utf-8').encode("gb2312")

    udp=dpkt.udp.UDP(dport=2425, sport=2425)  
    udp.data+=msg  	#传输层的数据
    udp.ulen=len(udp)  
      
    ip = dpkt.ip.IP(src=src_ip, dst=dst_ip, data=udp, p=dpkt.ip.IP_PROTO_UDP) 	#网络层的数据
   
    ip.len=len(ip)  #重新计算ip的长度,不然消息发送不出去  


    ether = dpkt.ethernet.Ethernet(  	#数据链路层的以太网帧
        dst = dst_mac,
        src = src_mac,  
        type = 0x0800,  
        data = ip  
    )
    device=sendpkt.findalldevs()[0]
    sendpkt.sendpacket(str(ether), device)  # 直接利用网卡。整个数据帧都是我构造的。


if __name__=="__main__":  
    s={  
       'src':'12.12.12.116',  
       'src_mac':'00-25-64-CA-9C-71',
       'host':'抖动之神',  
       'user':'NANKAI-9EFFB8A6',  
       
       'dst':'12.12.12.67', 'dst_mac':'00-25-64-CA-A4-D2', #我左边电脑
       #'dst':'12.12.12.116', 'dst_mac':'00-25-64-CA-9C-71', #peng

       'msg':''
    }
    
    send_msg(s)
    print get_local_mac()
