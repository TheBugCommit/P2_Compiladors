Limitacions: 
- Les potencies nomes poden tenir exponents enters

Decissions de disseny:
- He optat per declarar els arrays sense ; final, d'aquesta manera puc combinarlos amb les variables normals
- La base dels arrays s'incrementa per cada array declarat segons la seva mida * bytes del tipus de variable

Tot i no ser necessaries per aquesta pràctica vaig avançarme i implementar aquestes optimitzacions:
- Avaluació d'expressions aritmètiques que nomes tenen literals
- Reutilització de temporals
- Optimització algebràica 