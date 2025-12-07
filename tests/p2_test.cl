float x, y
int i , n, fact

x := 5.0
i := 3
y := i + i * 3 - x

n := 10
fact := 1
i := 2
repeat n - 1 do
    fact := fact * i
    i := i + 1
done

fact

/*
x := 5.0
i := 3
$t01 := i MULI 3
$t01 := i ADDI $t01
$t01 := I2F $t01
$t01 := $t01 SUBF x
y := $t01
n := 10
fact := 1
i := 2
$t01 := n SUBI 1
$t02 := 0
$t03 := fact MULI i
fact := $t03
$t03 := i ADDI 1
i := $t03
$t02 := $t02 ADDI 1
IF $t02 LTI $t01 GOTO 13
PARAM fact
CALL PUTI, 1
HALT
*/