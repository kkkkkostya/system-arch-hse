.include "macro-lib.asm"	

.eqv	NAME_SIZE 256 				# Размер буфера для имени файла
.eqv	TEXT_SIZE 4096 				# Размер буфера для текста

.data
prom:         .asciz "Input file name to write: "    			  # Название файла для записи

.text

.global create_new_file						 	# Создание нового файла
create_new_file:
	    print_n()
	    print_str (prom) 
	    read_str(file_out_name, NAME_SIZE)			# Считываем название файла для записи

	    # Убрать перевод строки
	    li  t4 '\n'
	    la  t5  file_out_name
	    mv  t3 t5					# Сохранение начала буфера для проверки на пустую строку
	    
	loop:
	    lb	t6  (t5)
	    beq t4	t6	replace
	    addi t5 t5 1
	    b   loop
	replace:
	    beq t3 t5 default				# Установка имени введенного файла
	    sb  zero (t5)
	    mv   a0, t3 				# Имя, введенное пользователем
	    b out
	default:
	    la   a0, default_name 			# Имя файла по умолчанию
	out:
		li   a7, 1024     	# system call for open file	
    		li   a1, 1        	# Open for writing (flags are 0: read, 1: write)
    		ecall             	# open a file (file descriptor returned in a0)
    		mv   	s6 a0       	# save the file descriptor
    		
    		# Write to file just opened
		li   	a7 64       	# system call for write to file
		mv   	a0 s6       	# file descriptor
		mv   	a1 a6   	# address of buffer from which to write
		li   	a2 44       	# hardcoded buffer length
		ecall             	# write to file
	
		# Close the file
		li   	a7 57       	# system call for close file
		mv   	a0 s6       	# file descriptor to close
		ecall             	# close file
ret
    			
    		
	
