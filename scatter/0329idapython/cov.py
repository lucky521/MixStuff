from idaapi import *
class FuncCov(DBG_Hooks):
	def dbg_bpt(self,tid,ea):
		print hex(ea)
		return
	def dbg_library_load(self, pid, tid, ea, name, base, size):
		print 'lucky: ' + name
		return


debugger = FuncCov()
debugger.hook()
current_addr = ScreenEA()
for f in Functions(SegStart(current_addr), SegEnd(current_addr)):
	#print f
	AddBpt(f)	#set breakpoint
	SetBptAttr(f, BPTATTR_FLAGS,0x0)


num_breakpoints = GetBptQty()	#return number of breakpoint
print "Set %d breakpoints." %num_breakpoints
