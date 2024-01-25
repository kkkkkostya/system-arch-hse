.include "macrlib.asm"	

	.data
inp: .asciz "Введите 0 чтобы ввести x или 1 чтобы протестировать программу  "
res: .asciz "tanh(x) =  "
sep: .asciz "\n"
ex: .asciz "Введено недопустимое значение!"
example: .asciz "x = "
tab: .asciz "   "
precision:
	.double 0.05 # точность
zerro:
	.double 0 
one:
 	.double 1
 hund:
 	.double 100

.text
.globl main
main:
	la a0, inp			# Ввывод указания
	li a7,4
	ecall
	
	li t2, 1
	read_int(s1)			# Ввести тип команды
	beqz s1, input_x		# if 0
	beq s1, t2, tests		# if 1 - go to tests
	j end				# else - end program
	
input_x:
	read_double (fa0)		# считываем x
	fld f1, zerro,  t0		
	fge.d s2, fa0, f1 		# sign x
	j initialization
	
.globl initialization
initialization:
	addi sp sp -4		# Запасаем одну ячейку для сохранения ra
	sw ra (sp)			# Сохраняем ra

	la a0, example		
	li a7, 4
	ecall
	
	li a7,3			# Ввыводим значение x
	ecall
	
	la a0, tab
	li a7, 4
	ecall

	fabs.d fa0,fa0 		# x = abs(x) тк ф-я симметрична
	fmv.d	f8,fa0  		# x
	fld 	f9, one, t0 		# k!
	fld	f18, zerro,  t0 	# tanh
	fld	f19, one,t0 		#e^x
	fld	f20, one, t0, 	#x^k
	fld	f21, zerro, t0,	#k
	fld     f22, precision, t0  # load precision
	fld	f23, one, t0
	fld	f24, hund, t0 
	
	fld f1, zerro, t0
	feq.d  t0, fa0, f1 		# if x = 0 - go to end
	bnez t0, fin
	
	j loop
	
	exi:				
		lw ra (sp)		# восстанавливаем ra из стека
		addi sp sp 4	# восстанавливаем вершину стека
		ret
	
loop:
	fadd.d f21, f21,f23	# увеличиваем значение k
	
	# функция для изменения значения x
	# новое значение x кладем в fa0
	# затем возвращаем его
	jal step_x			# изменяем значение x
	fmv.d f20, fa0		# сохраняем его
	
	# функция для подсчета факториала
	# новое значение кладем в fa0
	# затем возвращаем его
	jal fact			# считаем факториал
	fmv.d f9, fa0
	
	# функция для подсчета e^x
	# новое значение кладем в fa0
	# затем возвращаем его
	fdiv.d f1, f20,f9		# считаем новое слагаемое для e^x
	fadd.d f19, f19, f1	# изменяем значение e^x
	
	# функция для подсчета более точного значения tanh
	# новое значение кладем в fa0
	# затем возвращаем его
	jal update_tanh		# изменяем значение тангенса
	
	fsub.d f6, fa0, f18		# считаем на какой процент увеличлось новое более точное значение
	fdiv.d  f6, f6, f18
	fmul.d f6,f6,f24
	
	flt.d a0, f6, f22		# если значение точности не больше precision, тогда a0 = 1 
	
	fmv.d f18, fa0		# сохраняем новое значение tanh
	
	feq.d t0, f21, f23
	
	bnez t0, loop
	
	bnez a0, fin			# если точность уже нам подходит, то выходим из цикла
	
	j loop				# иначе продолжнаем считать
	
fin:
	la a0, res				
	li a7, 4
	ecall
	print	(f18, s2)				# выводим значение tanh
	beqz s1, exit				# если мы вводили значение x
	j exi						# если запускали testы
	
	
update_tanh:
	fdiv.d f2, f23,f19			# считаем e^(-x)
	fsub.d f3, f19, f2			# e^x - e^(-x) 
	fadd.d f4, f19,f2			# e^x + e^(-x) 
	fdiv.d f5, f3,f4			# считаем новое значение tanh
	fmv.d fa0, f5			
	ret
	
step_x:
	fmul.d fa0, f20, f8		# возводим x в степень k
	ret        
	
fact:
	fmul.d fa0, f9, f21		# вычисляем значение k!
	ret
	
end:
	la a0, ex				# если введено неправильное значение
	li a7,4
	ecall
	
	li a7, 10				# завершаем программу
	ecall	
	
exit:
	li a7, 10				# завершаем программу
	ecall	
	
	
        

	
	
