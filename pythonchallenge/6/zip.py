#--coding=utf-8--
'''
zipfile是python的一个用于快速处理zip压缩标准文件的模块。

zipfile.ZipFile(name):打开zip压缩文件
ZipFile.read(name[, pwd]):获取zip文档内指定文件的二进制数据
ZipFile.getinfo(name):获取zip文档内指定文件的信息

'''

import zipfile, re
result = file('result','w')
idx = "90052"

zfile = zipfile.ZipFile("channel.zip", "r")
while True:
	data = zfile.read(idx + ".txt")
	print idx + ":\t" + data
	
	idx = "".join(re.findall('[0-9.]', data))
	if len(idx) == 1:
		break
		
	result.write(zfile.getinfo(idx + '.txt').comment)
	
result.close()
