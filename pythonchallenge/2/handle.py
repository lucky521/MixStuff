#--coding=utf-8--


#http://www.pythonchallenge.com/pc/def/ocr.html


data = open("data")
result = open("result",'w')




#ch = 'a'
#print ord(ch)
#print chr(ord(ch))

while True:
	ch = data.read(1)
	if ch == '':
		break

	if ch >= 'a' and ch <= 'z':
		result.write(ch)


data.close()
result.close()
		
 

