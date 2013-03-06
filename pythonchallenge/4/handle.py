#--coding=utf-8--

import urllib,re

#http://www.pythonchallenge.com/pc/def/linkedlist.php
'''
用urllib来抓取网页。

16044/2 = 8022

'''


def getinto(num):
	global result
	google = urllib.urlopen('http://www.pythonchallenge.com/pc/def/linkedlist.php?nothing='+ num)

	for line in google:
		pattern = re.compile('[0-9]*[0-9]')
		match = pattern.search(line)
		
		if match:
			google.close()
			result.write(match.group()+'\n')
			getinto(match.group())
		
		
	


result = open("result",'w')


getinto(str(63579))


result.close()
		
 

