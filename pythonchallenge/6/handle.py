#--coding=utf-8--
#http://www.pythonchallenge.com/pc/def/channel.html
'''
Python Challenge level 6: “now there are pairs”
提示非常少
观察页面源码第一行
<!-- <-- zip -->
这个提示是啥意思捏。。
说明可能有一个zip文件。

不错，就在
http://www.pythonchallenge.com/pc/def/channel.zip

910个txt文本，其中一个是提示文件：
welcome to my zipped list.
hint1: start from 90052
hint2: answer is inside the zip

好吧，写代码吧。
'''

import re,sys
global name
name = '90052'
result = file('result','w')
result.write(name + '\n')

while True:
	data = file(name + '.txt')
	lnum = 0
	while 1:
		line = data.readline()
		if len(line) == 0:
			data.close()
			break
		lnum = lnum + 1
		if lnum > 1:
			print name + '\'s lnum > 1'
			
		pattern = re.compile('[0-9]*[0-9]')
		match = pattern.search(line)
		if match:
			name = match.group()
			result.write(line + '\n')
		else:
			print name + ' not match'
			print line
			sys.exit()
			

result.close()
	
