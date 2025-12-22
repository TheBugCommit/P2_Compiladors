int i, j, k
float x, y, z

i := 2
j := 3
k := 4
x := 1.5
y := 0.5

i := 10 + j * i ** j //34
k := 2 ** 3 ** 2 //512

j := 10 * k / i //150
k := 10 % j * k //5120

i := 10 + -2 // 8
j := 10 - -2 // 12
k := 10 * (-i) //-80
i := 10 / (+i) //1
j := -10 + 5 // -5

i := (10 + 2) * 3 // 36
j := 10 + (2 * 3) // 16
k := -(10 + 2) // -12

// ----- Proves d'error -----
i := 10 * (2 + 3 // Error: Manca parèntesi de tancament
i := 10 * 2 + // Error: Manca operand
i := (2 + 3) 5 // Error: Manca operador

/*
1:      i := 2
2:      j := 3
3:      k := 4
4:      x := 1.5
5:      y := 0.5
6:      $t01 := 1
7:      $t02 := 0
8:      IF $t02 GEI j GOTO 12
9:      $t01 := $t01 MULI i
10:     $t02 := $t02 ADDI 1
11:     GOTO 8
12:     $t01 := j MULI $t01
13:     $t01 := 10 ADDI $t01
14:     i := $t01
15:     k := 512
16:     $t01 := 10 MULI k
17:     $t01 := $t01 DIVI i
18:     j := $t01
19:     $t01 := 10 MODI j
20:     $t01 := $t01 MULI k
21:     k := $t01
22:     i := 8
23:     j := 12
24:     $t01 := CHSI i
25:     $t01 := 10 MULI $t01
26:     k := $t01
27:     $t01 := 10 DIVI i
28:     i := $t01
29:     j := -5
30:     i := 36
31:     j := 16
32:     k := -12
33:     i := 5
34:     HALT
*/