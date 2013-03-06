#--coding=utf-8--


#http://www.pythonchallenge.com/pc/def/equality.html
'''
One small letter, surrounded by 3 big letter on 
each of its sides.

aAAAaAAAa

[a-z][A-Z][A-Z][A-Z][a-z][A-Z][A-Z][A-Z][a-z]

正则表达式!!

'''
import re
data = open("data")
result = open("result",'w')


pattern = re.compile('(?<=[a-z][A-Z][A-Z][A-Z])[a-z](?=[A-Z][A-Z][A-Z][a-z])') 
#pattern = re.compile('[a-z][A-Z][A-Z][A-Z][a-z][A-Z][A-Z][A-Z][a-z]') 

while True:
	line = data.readline()
	if len(line) == 0:
		break
		
	match = pattern.search(line)

	if match:
		result.write(match.group())
		#result.write(" ")


data.close()
result.close()
		
 

