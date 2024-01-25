.include "macrlib.asm"	

.data
sep:    .asciz  "\n"		# Строка-разделитель (с \n и нулём в конце)
output: .asciz "\nÒåñòû: \n\n"
zerro:
	.double 0 
fir: .double -25
sec: .double -4
fird: .double -1
for: .double 0
fif: .double 0.3
six: .double 1.2
sev: .double 2.5
eig: .double 4.7

.text
.globl tests
tests:
	la a0, output
	li a7, 4
	ecall

	fld fa0, fir, t0			# считываем значение для теста
	fld f1, zerro,  t0			
	fge.d s2, fa0, f1 			# его знак
	
	# запустаем вычисление tanh для значения, находящегося в fa0
	# ф-я выводит посчитанное значение в консоль и ничего не возвращает	
	jal initialization 			# запускаем тест
	
	la a0, sep
	li a7,4
	ecall
        	
	fld fa0, sec, t0 			# считываем второе значение для теста
	fld f1, zerro,  t0
	fge.d s2, fa0, f1
       	jal initialization
       	
       	la a0, sep
	li a7,4
	ecall
        
       	fld fa0, fird, t0 
       	fld f1, zerro,  t0
	fge.d s2, fa0, f1
        	jal initialization
        	
        	la a0, sep
	li a7,4
	ecall
        
        	fld fa0, for, t0 
        	fld f1, zerro,  t0
	fge.d s2, fa0, f1
       	jal initialization
       	
       	la a0, sep
	li a7,4
	ecall
        
        	fld fa0, fif, t0 
        	fld f1, zerro,  t0
	fge.d s2, fa0, f1
      	jal initialization
      	
      	la a0, sep
	li a7,4
	ecall
        
       	fld fa0, six, t0 
       	fld f1, zerro,  t0
	fge.d s2, fa0, f1
        	jal initialization
        	
        	la a0, sep
	li a7,4
	ecall
        
        	fld fa0, sev, t0 
        	fld f1, zerro,  t0
	fge.d s2, fa0, f1
       	jal initialization
       	
       	la a0, sep
	li a7,4
	ecall
        
        	fld fa0, eig, t0 
        	fld f1, zerro,  t0
	fge.d s2, fa0, f1
        	jal initialization
        	
        	la a0, sep
	li a7,4
	ecall
        
	li a7, 10
	ecall 
