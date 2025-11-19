int i
float f
string s
bool b

i := 10
f := 1
f := 5.5
s := "Resultat: "
b := true

// Prova de promoció int -> float
// i + f -> 10 + 5.5 = 15.5 (float)
f := i + f
// f * i -> 5.5 * 10 = 55.0 (float)
f := f * i
// 10 / 4 = 2 (divisió entera)
i := 10 / 4
// 10.0 / 4 = 2.5 (divisió real)
f := 10.0 / 4
// 10 / 4.0 = 2.5 (divisió real)
f := 10 / 4.0

// Prova de concatenació (operador +)
// string + string
"Hola" + " " + "Mon"
// string + int
s := s + i // "Resultat: 2"
// int + string
s := i + s // "2Resultat: 2"
// string + float
s := s + f // "2Resultat: 22.5"
s := s + b // string + bool (Error)

// Prova de concatenació complexa
s := "i=" + i + ", f=" + f

// ----- Proves d'error semàntic -----
i := 10
f := 5.5
s := "hola"
b := true

i := s - "a" // Error: Operador '-' invàlid per a string
i := s * i // Error: Operador '*' invàlid per a string
i := s / i // Error: Operador '/' invàlid per a string
i := s % i // Error: Operador '%' invàlid per a string
i := s ** i // Error: Operador '**' invàlid per a string
i := i + b // Error: Operació aritmètica amb booleà
b := s + "a" // Error: Assignació de string a bool (l'expressió és vàlida)
i := f // Error: Assignació de float a int (requereix 'floor' o 'cast' explícit, que no existeix)
f := s // Error: Assignació de string a float