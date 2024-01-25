import math

a = [-25, -4, -1, 0, 0.3, 1.2, 2.5, 4.7]

for i in a:
    print(f"{'x = '+ str(i):<11}",end='')
    print(f"tanh(x) = {math.tanh(i)}")