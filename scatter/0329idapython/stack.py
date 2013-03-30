from idaapi import *
var_size_threshold = 16
current_addr = ScreenEA()

for f in Functions(SegStart(current_addr), SegEnd(current_addr)):
	stack_frame = GetFrame(f)	#get frame of stack
	frame_size = GetStrucSize(stack_frame)	#compute size of stackframe

	frame_counter = 0
	prev_count = -1
	distance = 0

	while frame_counter < frame_size:
		stack_var = GetMemberName(stack_frame, frame_counter) #get one from stack
		if stack_var != "":
			if prev_count != -1:
				distance = frame_counter - prev_distance
				prev_distance = frame_counter
				
				if distance >= var_size_threshold:
					print  "[*] Function: %s - > Stack Variable: %s (%d bytes)" % ( GetFunctionName(f), prev_member, distance )  
			else:
				prev_count = frame_counter
				prev_distance = frame_counter
				prev_member = stack_var

			try:
				frame_counter = frame_counter + GetMemberSize(stack_frame, frame_counter)	#compute offset
			except:
				frame_counter += 1
		else:
			frame_counter += 1
		

print 'over!'
