#--coding=utf-8--
'''
http://www.pythonchallenge.com/pc/def/peak.html
pickle module implements a fundamental but powerful algorithm for serializing a python object structure.
pickling is the process whereby a python object hierarchy is converted into a byte stream.
unpickling is the inverse operation whereby a byte stream is converted back into an object hierarchy.

隐藏了一个不显示的标签，下载下来。
<peakhell src="banner.p"/>

反序列化之后，得到了一个list。
然后就考察想象力了。
这个list其实是一张图。
'''

'''
#这是个学习pickle的例子
import StringIO
import pickle
class person(object):
	def __init__(self, name ,address):
		self.name = name
		self.address = address
	def display(self):
		print '*******************'
		print 'name    : ',self.name
		print 'address : ',self.address
		print '*******************'
		
lu = person('Lucky','China')
lu.display()
f1 = StringIO.StringIO()
pickle.dump(lu, f1, 0) #序列化
print f1.getvalue()
f1.seek(0)
lu = pickle.load(f1) #反序列化
lu.display()
f1.close()
'''

import cPickle as pickle
data = open('banner.p')
fo = open('out','w')
result = pickle.load(data)
#print type(result) #list

for line in result:
	for subline in line:
		#print type(subline) #tuple
		for i in range(0, subline[1]):
			fo.write(subline[0])
	fo.write('\n')
	

data.close()
fo.close()

