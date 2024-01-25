#Библиотека макросов
.eqv   STR_SIZE 4096
.eqv  inp_size 256

.data
quest: .asciz  "Нужно ли вывести результат?\nЕсли да, введите Y, иначе любой другой символ\n"
user_input:    .space STR_SIZE				# Размер строки для считывания
test_: .asciz "Запустить программу на тестах? Если на обычных тестах, то введите 0. Если на тестах с вызовом макросов - 1, иначе любой другой символ\n "
n: .asciz "\n"
user_test: .space inp_size


.macro print_or_not(%x)						# Макрос для вывода результата в консоль
print_n()
print_n()
print_str(quest)
print_n()
	
	read_str(user_input, STR_SIZE)				# Считывание запроса пользователя с консоли
	la %x user_input						
	lb      t6 (%x) 
	li 	t2 89
	la a0, new_str			
	beq	t6 t2 yes							# Если пользователь ввел Y
	j end
yes:
	print_n()
	print_str(new_str)
end:
.end_macro

# Считывание строки
.macro read_str(%x, %y)
   la	a0 %x
   li     a1 %y
   li      a7 8
   ecall
.end_macro

# Печать \n
.macro print_n()
   li a7, 4
   la a0, n
   ecall
.end_macro

# Печать строки
.macro print_str(%x)
   li a7, 4
   la a0, %x
   ecall
.end_macro

# Протестировать программу или ввести файл самостоятельно
.macro test_or_not()
   	print_str(test_)
   	read_str(user_test, inp_size)
   	la t1 user_test						
	lb      t6 (t1) 
	li 	t2 49			
	print_n()
	beq t6,t2,ok
	li 	t2 49	
	beq t6,t2, ok_2
	li a2, -1
	j end
ok:
	li a2, 0
	j end
ok_2:
	li a2, 1
end:
.end_macro

# Проверка, что символ является строчным гласным и возврат соответствующей буквы в заглавном регистре
.macro isVowel(%x)
   mv t2, %x
    
   li	a0 97					# Если символ - a
   beq %x, a0, vow
   
    li	a0 101				# Если символ - e
   beq %x, a0, vow			
   
    li	a0 105				# Если символ - i
   beq %x, a0, vow
   
    li	a0 111				# Если символ - o
   beq %x, a0, vow
   
    li	a0 117				# Если символ - u
   beq %x, a0, vow
   
    li	a0 121				# Если символ - y
   beq %x, a0, vow
   
    j end
vow:
	li t2, 32
	sub t2, a0, t2
   
end:
.end_macro

.macro call_read_file()
	jal read_file_
.end_macro

.macro call_string_processing()
	jal string_processing
.end_macro

.macro call_create_new_file()
	jal create_new_file
.end_macro
