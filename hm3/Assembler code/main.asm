.include "macro-lib.asm"

.eqv	NAME_SIZE 256 				# Размер буфера для имени файла
.eqv	TEXT_SIZE 4096 				# Размер буфера для текста
	
		.data
er_name_mes:    .asciz "Incorrect file name\n"
er_read_mes:    .asciz "Incorrect read operation\n"

new_str:     .space TEXT_SIZE				# Новая преобразованная строка
file_name:      .space	NAME_SIZE		# Имячитаемого файла
strbuf:	.space TEXT_SIZE			# Буфер для читаемого текста
default_name: .asciz "testout.txt"      	# Имя файла по умолчанию
file_out_name: .space	NAME_SIZE		# Имя читаемого файла
prompt:         .asciz "Input file path: "    			 # Путь до читаемого файла

.text
.global main file_name strbuf file_out_name default_name er_name_mes er_read_mes new_str
main:
	test_or_not()
	
	beqz a2, auto_test
	
	bgtz a2, auto_test_mac

	jal read_file_			# Ввод имени файла
	
	la a0, strbuf
	
	jal string_processing		# Обработка считанной строки
	
	print_or_not(t1)
	
	la a6, new_str
	jal create_new_file 		# Создание нового файла
	
	j exit
	
	auto_test:
		jal test_programm
		j exit
		
	auto_test_mac:
		jal test_programm_macros

exit:
	li a7, 10
	ecall
	
