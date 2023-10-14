# Macrob library

.macro fillel (%x) # Read elemnt
   li a7, 5
   ecall
   mv %x, a0
.end_macro

.macro printel (%x) # Print element
   mv a0, %x
   li a7, 1
   ecall
.end_macro

