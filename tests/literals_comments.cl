// Prova de comentaris
// Aquest és un comentari de línia
# Aquest és un altre estil de comentari de línia
/* Aquest és un comentari
   de múltiples línies */
int a, error1, error4 // Comentari al final d'una línia
float b /* comentari a mitja línia */ 
float c
float f1, f2, f3
string s1, error2, error3
int i1, i2
bool b1, b2

// Prova de literals enters
i1 := 123
i2 := 045
i1 := 0

f1 := 3.1416
f2 := 0.5e-04
f3 := 1.2345e3
f1 := 1. // Un real que acaba en punt

// Prova de literals de cadena
s1 := "Hola món"
s1 := "Cadena amb \" cometes escapades (si s'implementa)"
s1 := "" // Cadena buida

// Prova de literals booleans
b1 := true
b2 := false

// ----- Proves d'error lèxic -----
error1 := 1.2.3 // Error: Real mal format
error2 := "Cadena sense tancar
// Error: Cadena multi-línia no permesa
error3 := "Cadena amb salt de línia \n" // Error: (si no es permeten escapes)
error4 := 0.5e- // Error: Exponent mal format
/* Comentari sense tancar
// Error: EOF dins d'un comentari