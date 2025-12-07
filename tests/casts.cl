int x
float y

//Implicits
x := 3
y := 5 + x * (-2.5)

// Explicits
x := 1
x := int(3.1416)
x := int(y)
y := float(x)
y := float(2)

//errors
y := x

/*
x := 3
$t01 := I2F x
$t01 := $t01 MULF -2.5
$t01 := 5.0 ADDF $t01
y := $t01
x := 1
x := 3
$t01 := F2I y
x := $t01
$t01 := I2F x
y := $t01
y := 2.0
HALT
*/