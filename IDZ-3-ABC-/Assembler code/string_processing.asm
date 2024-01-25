.include "macro-lib.asm"	

.data
prom:         .asciz "Input file name to write: "    			  # Название файла для записи

.text

.global string_processing						 	# Обработка строки согласно условию задания
string_processing:
	mv t5, a0
	do:   
		la      t6 new_str
		li 	t0 0
		
	prepr:
	      	lb      t1 (t5) 
	 
	      	isVowel(t1)							# Преобразование символа если он явлется строчным гласным
		sb	t2 (t6)							# Загружаем символ в новую строку
		li 	t0 1
		addi    t6 t6 1 
	          beqz    t1 end 
	          addi	t5 t5 1
	          b       prepr
	        
	end:	
		addi	t6 t6 -1
		li	t4 0
		sb	t4 (t6)
	ret 
