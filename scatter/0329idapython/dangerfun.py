from idaapi import *
f1 = file('danger_call.txt','w')
danger_funcs = ["strcpy","sprintf","strncpy"]
for func in danger_funcs:
	addr = LocByName(func)
	if addr != BADADDR:
		cross_refs = CodeRefsTo(addr, 0)
		f1.write("################\n")
		for ref in cross_refs:
			f1.write(str(hex(ref)) + '\n')

f1.close()