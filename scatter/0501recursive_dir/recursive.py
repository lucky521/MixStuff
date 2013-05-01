#--coding=UTF8--
import os
src_dir = "lucky_src"
dst_dir = "lucky_dst"



def stemming(cpwd, dpwd):
	if os.path.isdir(cpwd) == True:
		os.mkdir(os.path.join(dpwd, cpwd))
		for p in os.listdir(cpwd):
			tmp_pwd = os.path.join(cpwd, p)
			stemming(tmp_pwd, dpwd)

	else:
		#os.system("stem.exe  " + str(cpwd) + " > " + os.path.join(dpwd, cpwd) )
		os.system("echo " + str(cpwd) + " > " + os.path.join(dpwd, cpwd) )
		


try:
	os.mkdir(dst_dir)
except:
	print "[Error]:Dst Dir Already exists!"
	exit()
	
src_pwd = src_dir
dst_pwd = dst_dir

stemming(src_pwd, dst_pwd)
print "[Done]"
