import os

file_name = file("file_name","w")

pwd = os.getcwd()
#pwd = pwd + "/dataset"
pwd = os.path.join(pwd, "dataset")


p_dir = os.listdir(pwd)

file_index = -1

for p in p_dir:
	cpwd = os.path.join(pwd, p)
	c_dir = os.listdir(cpwd)
	
	for c in c_dir:
		file_index = file_index + 1
		file_name.write(str(file_index) + "  " + os.path.join(cpwd, c) + '\n')


file_name.close()
