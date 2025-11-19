// Declaracions simples
int i
float f
string s
bool b

// Declaracions múltiples
int i1, i2, i3
float f1, f2
string s1, s2, s3, s4
bool b1, b2

// ----- Proves d'error sintàctic/semàntic -----
int i, float f // Error: No es poden barrejar tipus en una línia
int // Error: Manca identificador

float 1f // Error: Identificador invàlid (comença amb número)
int i, i // Error: Identificador redeclarat
string i // Error: Identificador redeclarat amb diferent tipus
int x
x := 10
int x // Error: Redeclaració després d'ús (o en general)