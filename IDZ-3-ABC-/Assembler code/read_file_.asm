.include "macro-lib.asm"	

.eqv	NAME_SIZE 256 						# Размер буфера для имени файла
.eqv	TEXT_SIZE 4096 						# Размер буфера для текста	

.data
er_name_mes:    .asciz "Incorrect file name\n"
prompt:         .asciz "Input file path: "    			 # Путь до читаемого файла

.text
.global read_file_							# Подпрограмма для считывания файла
read_file_:
	print_str (prompt) 
	read_str(file_name, NAME_SIZE)			# Считывание названия файла
	
	# Убрать перевод строки
   	li	t4       '\n'
    	la	t5	file_name
    	
	
	loop:
	    lb	t6  (t5)
	    beq t4	t6	replace
	    addi t5 t5 1
	    b	loop
	    
	    
	replace:
		    sb	zero (t5)
		
		    li   	a7 1024     					# Системный вызов открытия файла
		    la      a0 file_name    				# Имя открываемого файла
		    li   	a1 0        					# Открыть для чтения (флаг = 0)
		    ecall             					# Дескриптор файла в a0 или -1)
		    li		t3 -1					# Проверка на корректное открытие
		    beq	a0 t3 er_name			# Ошибка открытия файла
		   mv   	s0 a0       				# Сохранение дескриптора файла
		   
	    li   a7, 63       						# Системный вызов для чтения из файла
	    mv a0, s0
	    la   a1, strbuf   						# Адрес буфера для читаемого текста
	    li   a2, TEXT_SIZE 					# Размер читаемой порции
	    ecall             						# Чтение
	   
	    # Проверка на корректное чтение
	    beq	a0 t3 er_read				# Ошибка чтения
	    mv   	s2 a0       					# Сохранение длины текста
	    
	    # Закрытие файла
	    li   a7, 57       # Системный вызов закрытия файла
	    mv a0, s0
	    ecall             # Закрытие файла
	    
	    # Установка нуля в конце прочитанной строки
	    la	t0 strbuf	 # Адрес начала буфера
    	    add t0 t0 s2	 # Адрес последнего прочитанного символа
    	    addi t0 t0 1	 # Место для нуля
    	    sb	zero (t0)	 # Запись нуля в конец текста
    	    
    	    print_n()
    	    la a0, strbuf
    	    li a7, 4
    	    ecall
    	    
    	    ret
	    
er_read:
	    # Сообщение об ошибочном чтении
	    la		a0 er_read_mes
	    li		a7 4
	    ecall
	    ret
		    
er_name:
	    # Сообщение об ошибочном имени файла
	    la		a0 er_name_mes
	    li		a7 4
	    ecall
	    # И завершение программы
	    li		a7 10
	    ecall
