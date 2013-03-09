#create a random ID number
#file "location" is database of location-number
import random, re
def build_location():	#6
	data = open('location')
	r = random.randint(1, 3519)
	for i in range(1, r):
		line = data.readline()
	data.close()
	pattern = re.compile('\d{6}')
	match = pattern.search(line)
	if match:
		return match.group()
	
def build_birth():	#8
	year = random.randint(1970, 1990)
	month = random.randint(0, 12)
	month = "%02d" %month
	day = random.randint(0, 28)
	day = "%02d" %day
	birth = str(year) + str(month) + str(day)
	return birth

def build_third():	#3
	a = random.randint(000, 999)
	a = "%03d" %a
	return str(a)

def compute_last(num_before):	#1
	weight = [7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2]
	endone = [1, 0, 'X', 9, 8, 7, 6, 5, 4, 3, 2]
	s = 0
	for i in range(0, 17):
		n = int(num_before[i])
		s = s + n*weight[i]
	return str(endone[s%11]) 

##########################################
myloc = build_location()
mybirth = build_birth()
mythird = build_third()
num_before = myloc + mybirth + mythird
last_one = compute_last(num_before)

myid = num_before + last_one
print myid
##########################################
