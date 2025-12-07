int x, y
x := 1
y := 1
repeat 4 do
    int z
    z := x
    repeat 3 do 
        y := x * 2
    done
    x := x + 1
    y := y + x
done

/*
x := 1
y := 1
$t01 := 0
z := x
$t02 := 0
$t03 := x ADDI x
y := $t03
$t02 := $t02 ADDI 1
IF $t02 LTI 3 GOTO 6
$t02 := x ADDI 1
x := $t02
$t02 := y ADDI x
y := $t02
$t01 := $t01 ADDI 1
IF $t01 LTI 4 GOTO 4
HALT
*/