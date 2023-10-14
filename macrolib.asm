# Библиотека макросов

.macro fillel (%x) # Считываем элемент
   li a7, 5
   ecall
   mv %x, a0
.end_macro

.macro printel (%x) # Выводи элемент
   mv a0, %x
   li a7, 1
   ecall
.end_macro

