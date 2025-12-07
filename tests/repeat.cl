int x, y
x := 1
y := 1
repeat 4 do
    repeat 3 do 
        y := x * 2
    done
    x := x + 1
    y := y + x
done