int a[20], b, c[30]
float d[20], e, f[30]
b := 2
a[0] := 1
c[1] := 1 * 5
c[2] := b + c[a[0] + 5]
d[0] := float(c[2] + c[1])

a := int(d)

// Errors
float x[-2]
float y[0]
d := 1
f[1]

/*
b := 2
a[145] := 1
c[29] := 5
$t01 := a[145]
$t01 := $t01 ADDI 5
$t01 := $t01 MULI 4
$t01 := 25 ADDI $t01
$t01 := c[$t01]
$t01 := b ADDI $t01
c[33] := $t01
$t01 := c[33]
$t02 := c[29]
$t01 := $t01 ADDI $t02
$t01 := I2F $t01
d[345] := $t01
$t01 := F2I d
a := $t01
HALT
*/