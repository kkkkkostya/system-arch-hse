.include "macro-lib.asm"

.global test_programm
.text
test_programm:							# Тестовая программа
	li s10, 0
	li s11, 4
	
	addi sp sp -4		# Запасаем одну ячейку для сохранения ra
	sw ra (sp)			# Сохраняем ra
	loopp:
	
		jal read_file_			# Ввод имени файла
	
		la a0, strbuf
		
		jal string_processing		# Обработка считанной строки
		
		print_or_not(t1)
		
		la a6, new_str
		jal create_new_file 		# Создание нового файла
	
		addi s10,s10,1
		beq s10,  s11,ex
		j loopp
		
	ex:
		lw ra (sp)		# восстанавливаем ra из стека
		addi sp sp 4	# восстанавливаем вершину стека
		ret
		
