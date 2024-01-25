# Macrob library

.macro read_double (%x) # Read double elemnt
   li a7, 7
   ecall
   fmv.d  %x f10	# move element to x
   
.end_macro

.macro read_int (%x) # Read int elemnt
   li a7, 5
   ecall
   mv  %x a0	# move element to x
   
.end_macro

.macro print (%x,%t) # Print double element
   mv t1, %t
   fmv.d f10, %x	# move x to f10
   bgtz t1, cont 	# if x>0
   fneg.d f10, f10 # change sigh if x<0
   cont:
   	li a7, 3
   	ecall
.end_macro

