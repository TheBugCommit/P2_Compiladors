int i, j, k
float f, g

i := 5
f := 5.5

// 1. CONSTANT FOLDING

j := 10 + 20            // C3A: j := 30
g := 2.5 * 4.0          // C3A: g := 10.0
j := 2 ** 3             // C3A: j := 8 (Calculat amb pow() de C)
g := 2 ** (-2)          // C3A: g := 0.25 (Detecta exp negatiu i passa a float)
j := -5                 // C3A: j := -5 (Unari constant)

// 2. OPTIMITZACIONS ALGEBRAIQUES (Identitats)

j := i + 0              // Opt: x+0=x. C3A: j := i
j := 0 + i              // Opt: 0+x=x. C3A: j := i
j := i - 0              // Opt: x-0=x. C3A: j := i
j := i * 1              // Opt: x*1=x. C3A: j := i
j := 1 * i              // Opt: 1*x=x. C3A: j := i
j := i / 1              // Opt: x/1=x. C3A: j := i
j := i * 0              // Opt: x*0=0. C3A: j := 0
j := 0 * i              // Opt: 0*x=0. C3A: j := 0
j := i % 1              // Opt: x%1=0. C3A: j := 0

// 3. REDUCCIÓ DE FORÇA
j := i * 2              // Opt: x*2 -> x+x. C3A: $t01 := i ADDI i
j := 2 * i              // Opt: 2*x -> x+x. C3A: $t01 := i ADDI i
j := i ** 2             // Opt: x^2 -> x*x. C3A: $t01 := i MULI i

// 4. POTÈNCIA: LOOP UNROLLING (n <= 4)
j := i ** 0             // Opt: x^0=1. C3A: j := 1
j := i ** 1             // Opt: x^1=x. C3A: j := i
j := i ** 3             // Opt: Unroll. C3A: t1:=i*i; t1:=t1*i (2 MULI)
j := i ** 4             // Opt: Unroll. C3A: 3 MULI successius

// 5. POTÈNCIA: RUNTIME LOOP (n > 4 o Var)
j := i ** 5             // No Opt (Literal gran). C3A: IF...GOTO...
k := 3
j := i ** k             // No Opt (Exponent variable). C3A: IF...GOTO...

// 6. GESTIÓ DE TIPUS I MIXTES
g := f + 2              // Float + Int. C3A: t1:=f ADDF t1
g := i + 2.5            // Int + Float. C3A: t1:=I2F i; g:=t1 ADDF 2.5

// 7. PRIORITATS I COMPLEXITAT
j := i + i * 2          // Prioritat: * abans +. I 'i*2' es redueix a 'i+i'.
                        // C3A: t1:=i ADDI i; j:=i ADDI t1

g := (f + 1.0) ** 2     // Parèntesis + Reducció de Força.
                        // C3A: t1:=f ADDF 1.0; t2:=t1 MULF t1; g:=t2