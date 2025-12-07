int x, a[1]
float y, b[1]

x := 1
y := 3.12
x
y

a[0] := 1
b[0] := 3.12
a[0]
b[0]

//Errors
a

/*
x := 1
y := 3.12
PARAM x
CALL PUTI, 1
PARAM y
CALL PUTF, 1
a[25] := 1
b[29] := 3.12
$t01 := a[25]
PARAM $t01
CALL PUTI, 1
$t01 := b[29]
PARAM $t01
CALL PUTF, 1
HALT
*/