#--coding=utf-8--


#http://www.pythonchallenge.com/pc/def/ + ~

#最简单的加密方法
#26字母表循环右移2位
import string
data = open("data")
result = open("result",'w')




#ch = 'a'
#print ord(ch)
#print chr(ord(ch))

while True:
	ch = data.read(1)
	if ch == '':
		break

	if ch >= 'a' and ch <= 'x':
		ch = chr(ord(ch) + 2)		
	elif ch == 'y' :
		ch = 'a'
	elif ch == 'z':
		ch = 'b'
	else:
		pass
		
	result.write(ch)


data.close()
result.close()
		
 

