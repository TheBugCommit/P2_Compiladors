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
