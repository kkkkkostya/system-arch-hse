.include "macrlib.asm"	

	.data
inp: .asciz "������� 0 ����� ������ x ��� 1 ����� �������������� ���������  "
res: .asciz "tanh(x) =  "
sep: .asciz "\n"
ex: .asciz "������� ������������ ��������!"
example: .asciz "x = "
tab: .asciz "   "
precision:
	.double 0.05 # ��������
zerro:
	.double 0 
one:
 	.double 1
 hund:
 	.double 100

.text
.globl main
main:
	la a0, inp			# ������ ��������
	li a7,4
	ecall
	
	li t2, 1
	read_int(s1)			# ������ ��� �������
	beqz s1, input_x		# if 0
	beq s1, t2, tests		# if 1 - go to tests
	j end				# else - end program
	
input_x:
	read_double (fa0)		# ��������� x
	fld f1, zerro,  t0		
	fge.d s2, fa0, f1 		# sign x
	j initialization
	
.globl initialization
initialization:
	addi sp sp -4		# �������� ���� ������ ��� ���������� ra
	sw ra (sp)			# ��������� ra

	la a0, example		
	li a7, 4
	ecall
	
	li a7,3			# �������� �������� x
	ecall
	
	la a0, tab
	li a7, 4
	ecall

	fabs.d fa0,fa0 		# x = abs(x) �� �-� �����������
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
		lw ra (sp)		# ��������������� ra �� �����
		addi sp sp 4	# ��������������� ������� �����
		ret
	
loop:
	fadd.d f21, f21,f23	# ����������� �������� k
	
	# ������� ��� ��������� �������� x
	# ����� �������� x ������ � fa0
	# ����� ���������� ���
	jal step_x			# �������� �������� x
	fmv.d f20, fa0		# ��������� ���
	
	# ������� ��� �������� ����������
	# ����� �������� ������ � fa0
	# ����� ���������� ���
	jal fact			# ������� ���������
	fmv.d f9, fa0
	
	# ������� ��� �������� e^x
	# ����� �������� ������ � fa0
	# ����� ���������� ���
	fdiv.d f1, f20,f9		# ������� ����� ��������� ��� e^x
	fadd.d f19, f19, f1	# �������� �������� e^x
	
	# ������� ��� �������� ����� ������� �������� tanh
	# ����� �������� ������ � fa0
	# ����� ���������� ���
	jal update_tanh		# �������� �������� ��������
	
	fsub.d f6, fa0, f18		# ������� �� ����� ������� ���������� ����� ����� ������ ��������
	fdiv.d  f6, f6, f18
	fmul.d f6,f6,f24
	
	flt.d a0, f6, f22		# ���� �������� �������� �� ������ precision, ����� a0 = 1 
	
	fmv.d f18, fa0		# ��������� ����� �������� tanh
	
	feq.d t0, f21, f23
	
	bnez t0, loop
	
	bnez a0, fin			# ���� �������� ��� ��� ��������, �� ������� �� �����
	
	j loop				# ����� ����������� �������
	
fin:
	la a0, res				
	li a7, 4
	ecall
	print	(f18, s2)				# ������� �������� tanh
	beqz s1, exit				# ���� �� ������� �������� x
	j exi						# ���� ��������� test�
	
	
update_tanh:
	fdiv.d f2, f23,f19			# ������� e^(-x)
	fsub.d f3, f19, f2			# e^x - e^(-x) 
	fadd.d f4, f19,f2			# e^x + e^(-x) 
	fdiv.d f5, f3,f4			# ������� ����� �������� tanh
	fmv.d fa0, f5			
	ret
	
step_x:
	fmul.d fa0, f20, f8		# �������� x � ������� k
	ret        
	
fact:
	fmul.d fa0, f9, f21		# ��������� �������� k!
	ret
	
end:
	la a0, ex				# ���� ������� ������������ ��������
	li a7,4
	ecall
	
	li a7, 10				# ��������� ���������
	ecall	
	
exit:
	li a7, 10				# ��������� ���������
	ecall	
	
	
        

	
	
