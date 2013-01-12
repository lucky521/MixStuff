#coding=utf-8
from __future__ import division
import sys
reload(sys)
sys.setdefaultencoding('utf8')
exlist = ['\n','，','。','！','？','1','2','3','4','5','6','7','8','9','0']
word = ''
bad_file = ['e1','e2']
good_file = ['e3','e4']
#######################################################Learning
for name in good_file:
	f = file(name)
	ch = f.read()
	ch = unicode(ch, 'utf-8')
	for i in range(len(ch)):
		if ch[i] in exlist:
			continue
		else:
			word += ch[i]
	f.close()
#print 'Word\'s Num:',len(word)
#print word
##############compute prequency
good_dict = {} 
for i in range(len(word)):
	le = word[i]
	if good_dict.has_key(le):
		good_dict[le] += 1
	else:
		good_dict[le] = 1

		
for k in good_dict.iterkeys():
	good_dict[k] = good_dict[k] / len(good_dict)

print '####################################'
print 'Good Email Hash:'
for k in good_dict.iterkeys():
	print k,' : ',good_dict[k]

######################################################
word = ''
for name in bad_file:
	f = file(name)
	ch = f.read()
	ch = unicode(ch, 'utf-8')
	for i in range(len(ch)):
		if ch[i] in exlist:
			continue
		else:
			word += ch[i]
	f.close()
#print 'Word\'s Num:',len(word)
#print word
##############compute prequency
bad_dict = {} 
for i in range(len(word)):
	le = word[i]
	if bad_dict.has_key(le):
		bad_dict[le] += 1
	else:
		bad_dict[le] = 1

for k in bad_dict.iterkeys():
	bad_dict[k] = bad_dict[k] / len(bad_dict)

print '####################################'
print 'Bad Email Hash:'
for k in bad_dict.iterkeys():
	print k,' : ',bad_dict[k]


###########################################################Hashtable_probability
proba = {}
for le in bad_dict.iterkeys():
	if good_dict.has_key(le):
		proba[le] = bad_dict[le] / (bad_dict[le] + good_dict[le])
	else:
		proba[le] = 1
print '####################################'
print 'Probability Hash:'
for k in proba.iterkeys():
	print k,' : ',proba[k]
############################################################Test a new email
f = file('test')
word = ''
ch = f.read()
ch = unicode(ch, 'utf-8')
for i in range(len(ch)):
	if ch[i] in exlist:
		continue
	else:
		word += ch[i]
f.close()

pr = [ 0 for i in range(len(word)) ]
j = 0
for i in range(len(word)):
	if proba.has_key(word[i]):
		pr[j] = proba[word[i]]
	else:
		pr[j] = 0
	j = j + 1
	
p1 = 1
p2 = 1
for i in range(len(pr)):
	p1 =  p1 * pr[i]
	p2 =  p2 * (1 - pr[i])

probability = p1 / (p1 + p2)
print '########################################'
print 'probability: ',probability

		
	
		


