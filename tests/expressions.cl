int a
float b, c
string s, s_empty
bool l, t, f

# basic assig 
a := 5
b := 3.14
c := -2.71
s := "Hola"
s_empty := ""
l := true
t := true
f := false

# literals and unary
-5 //-5
+10 //10
-(+3) //-3
-(-5) //5
+(-(-3)) //3
---5   //-5
++-+-3 //3

# basic arith 
a + 2 //7
b * 2.0 //6.28
c / 2 //-1.355
a % 2 //1
2 ** 3 //8
(-2) ** 3 //-8
(-2) ** 4 //16
-(+3*(-2)) * (-2) * (-5) //60

# identifiers in expressions
a + b //8.14
c * a - b / 2 //-15.12
a + b * c - 1 //-4.5094
-c + b // 5.85
(a + b) * (c - 1) //-30.1994

# strings
s + " Mundo" // "Hola Mundo"
"Hola" + " " + "Mundo" // "Hola Mundo"
s_empty + "test" // "test"
"X" + s_empty + "Y" //"XY"

# booleans
a > 2 //true
b <= c //false
l and false //false
a <> 10 //true
not l //false
a > 1 and b < 4 //true
l or false //true
not a > 5 or l and true //true
a = a //true
1 = b //false

# bolean parentesis
(1 < 2) and (3 > 1) //true
not (l or false) //false
not (l or false) and (a > 0) //false

#precedence order
1 + 2 * 3 //7
(1 + 2) * 3 //9
2 ** 3 ** 2  //512       
-(2 ** 3) ** 2 //-64
1 + 2 * 3 ** 2 //19
(1 + 2 * 3) ** 10 //282475249
not false and true or false //true

# assignations
a := (a + 2) * 3 //21
b := b / (1 + 1) //1.57
s := s + "!!!" // "Hola!!!"
l := not (a < 0) //true

#  Special cases
0 ** 0 // 1
a / 0        
a % 0        
"Hola" + 5   
true + 1     
"Test" ** 2  
