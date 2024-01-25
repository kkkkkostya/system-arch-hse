.include "macro-lib.asm"	

.eqv	NAME_SIZE 256 						# ������ ������ ��� ����� �����
.eqv	TEXT_SIZE 4096 						# ������ ������ ��� ������	

.data
er_name_mes:    .asciz "Incorrect file name\n"
prompt:         .asciz "Input file path: "    			 # ���� �� ��������� �����

.text
.global read_file_							# ������������ ��� ���������� �����
read_file_:
	print_str (prompt) 
	read_str(file_name, NAME_SIZE)			# ���������� �������� �����
	
	# ������ ������� ������
   	li	t4       '\n'
    	la	t5	file_name
    	
	
	loop:
	    lb	t6  (t5)
	    beq t4	t6	replace
	    addi t5 t5 1
	    b	loop
	    
	    
	replace:
		    sb	zero (t5)
		
		    li   	a7 1024     					# ��������� ����� �������� �����
		    la      a0 file_name    				# ��� ������������ �����
		    li   	a1 0        					# ������� ��� ������ (���� = 0)
		    ecall             					# ���������� ����� � a0 ��� -1)
		    li		t3 -1					# �������� �� ���������� ��������
		    beq	a0 t3 er_name			# ������ �������� �����
		   mv   	s0 a0       				# ���������� ����������� �����
		   
	    li   a7, 63       						# ��������� ����� ��� ������ �� �����
	    mv a0, s0
	    la   a1, strbuf   						# ����� ������ ��� ��������� ������
	    li   a2, TEXT_SIZE 					# ������ �������� ������
	    ecall             						# ������
	   
	    # �������� �� ���������� ������
	    beq	a0 t3 er_read				# ������ ������
	    mv   	s2 a0       					# ���������� ����� ������
	    
	    # �������� �����
	    li   a7, 57       # ��������� ����� �������� �����
	    mv a0, s0
	    ecall             # �������� �����
	    
	    # ��������� ���� � ����� ����������� ������
	    la	t0 strbuf	 # ����� ������ ������
    	    add t0 t0 s2	 # ����� ���������� ������������ �������
    	    addi t0 t0 1	 # ����� ��� ����
    	    sb	zero (t0)	 # ������ ���� � ����� ������
    	    
    	    print_n()
    	    la a0, strbuf
    	    li a7, 4
    	    ecall
    	    
    	    ret
	    
er_read:
	    # ��������� �� ��������� ������
	    la		a0 er_read_mes
	    li		a7 4
	    ecall
	    ret
		    
er_name:
	    # ��������� �� ��������� ����� �����
	    la		a0 er_name_mes
	    li		a7 4
	    ecall
	    # � ���������� ���������
	    li		a7 10
	    ecall
