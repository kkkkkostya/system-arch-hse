# Библиотека макросов

.macro fillel (%x) # Считывание элемента
   li a7, 5
   ecall
   mv %x, a0
.end_macro

.macro printel (%x) # Вывод элемента
   mv a0, %x
   li a7, 1
   ecall
.end_macro

