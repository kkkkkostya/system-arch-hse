.include "macro-lib.asm"

.global test_programm_macros
.text
test_programm_macros:							# Тестовая программа
	li s10, 0
	li s11, 2
	
	addi sp sp -4		# Запасаем одну ячейку для сохранения ra
	sw ra (sp)			# Сохраняем ra
	loopp:
	
		call_read_file()			# Ввод имени файла
	
		la a0, strbuf
		
		call_string_processing		# Обработка считанной строки
		
		print_or_not(t1)
		
		la a6, new_str
		call_create_new_file 		# Создание нового файла
	
		addi s10,s10,1
		beq s10,  s11,ex
		j loopp
		
	ex:
		lw ra (sp)		# восстанавливаем ra из стека	
		addi sp sp 4		# восстанавливаем вершину стека
		ret
		
