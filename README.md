//314CAb-stafi Tudor Vasile
Descriere program:
0. Programul citeste un sir de caractere pe post
de comanda, extrage informatiile necesare din
el, de exemplu din comanda LOAD va extrage numele
fisierului pentru a putea fi prelucrat ulterior.
Sunt declarate si 2 structuri, una contine datele din
matrice si una pentru dimensiunile acesteia si magic word.
Structura are 4 parametri pentru cele 4 culori posibile
RGB sau gri.
1. Comanda LOAD
Comanda LOAD, in cazul in care trece de teste 
(de ex daca file-ul introdus exista), va citi 
in mod general, cu "r" din fisier cele 4 valori
(magic word, width etc.) si va deschide din 
nou fisierul pentru a-l putea citi in functie
de magic word. Daca este un file binar, se va citi 
un caracter gol pentru a nu se converti \n si a
se pune pe prima pozitie in matrice. Daca este 
text, file-ul se va deschide cu r, iar cu rb in 
cazul in care este binar. In functie
si de faptul ca magic wordul poate determina o 
imagine rgb sau gri se vor citi cate 3 valori
sau cate una pentru fiecare pixel si se va salva
astfel totul intr-o matrice alocata dinamic.
2. Comanda SAVE 
Aceasta comanda este aproximativ identica la nivel
de structura cu LOAD, doar ca in functie de prezenta
sau absenta cuvantului ascii din comanda tastata se
va folosi fprintf sau, in caz de binar, fwrite.
Daca initial textul a fost citit binar si se cere
salvare in mod ascii sau invers, se va modifica si 
magic wordul corespunzator (din P2 in P5, P3 in P6 sau
inversele).
3. Comanda SELECT ALL
Aceasta comanda este cea mai simpla si afiseaza
mesajul corespunzator(daca exista sau nu un fisier
incarcat sau daca s-a efectuat comanda), si care
ii atribuie lui h1(inaltimea coltului din stanga
al matricei) si lui w1(latimea coltului din stanga
al matricei) valoarea 0, si lui h2(coltul din dreapta)
si w2(coltul din dreapta) valoarea lui savex, si savey,
care sunt coordonatele citite in load si care pot fi 
modificate doar prin CROP sau ROTATE.
4. Comanda SAVE
Aceasta atribuie lui h1,w1,h2,w2 cele 4 valori
citite, daca sunt valide. Daca nu, Valorile vor 
avea datele primite la inceput sau de la ultimul
select.
5. Comanda HISTOGRAM
Aceasta calculeaza initial frecventele pixelilor
din matrice. Ulterior, daca numarul de bins nu este 
egal cu 256, se vor aduna cate 256/bins valori 
din vectorul de frecventa intr-un alt vector final si
in timp ce se face asta se calculeaza si maximul
acestor sume pentru formula de calculare a stelutelor.
Se afiseaza la final, conform formatului din cerinta,
numarul de stele calculate dupa formula.
6. Comanda EQUALIZE
Se calculeaz ca la histogram, intai frecventa fiecarui pixel
din matrice. Apoi se calculeaza pentru fiecare pixel suma 
tuturor pixelilor mai mici sau egali cu el, si se aplica
formula. Rezultatul se va salva intr-o matrice auxiliara,
pentru a nu se modifica definitiv cea initiala si pentru
a se putea efectua fiecare calcul in mod corespunzator,
utilizandu-se si round sau clamp pentru a nu depasi [0,255].
Ulterior, cea auxiliara o va inlocui pe cea initiala, 
iar spatiul celei initiale va fi eliberat.
7. Comanda CROP
Aceasta comanda calculeaza dimensiunile unei matrice noi,
(h2-h1 e inaltimea noua, si w2-w1 e latimea noua), si copiaza
in ea doar portiunea de la h1 la h2 si w1 la w2(daca nu s-a dat 
un select in prealabil, se va copia practic toata matricea
si cropul nu va avea niciun efect). La finalul copierii
se va elibera matricea initiala si se va atribui pointerul 
catre adresa celei noi.
8. Comanda ROTATE
Avand in vedere ca o matrice rotita la +/-90 grade este
identica cu una rotita cu -/+270 grade, una rotita la 0
sau +-360 este identica cu cea initiala, iar cea rotita
la -/+180 grade este identica cu una rotita de 2 ori in
oricare directie, am facut o functie care roteste o matrice
la 90 grade la stanga si am aplicat-o de atatea ori cat a
fost nevoie(pentru -270 de 3 ori, pentru +270 o data etc.).
Astfel, daca matricea trece de teste(daca selectul nu e 
patratic etc.) se vor face urmatoarele operatii:
Se va aloca o matrice de dimensiuni inverse
(ca o transpusa doar ca liniile sunt putin altfel), 
si se copiaza in ea valori dupa formula:
a[coloane-1-j][i]=b[i][j];
Se verifica apoi daca a fost dat un select. In caz 
afirmativ doar se vor copia valorile in matrice si se 
va elibera matricea noua, altfel se va elibera cea initiala
si i se va atribui matricei vechi adresa celei noi.
9. Comanda APPLY
Se apeleaza o functie care va atribui unei matrice kernel
valorile corespunzatoare parametrului extras din comanda
(BLUR EDGE etc.). Apoi se va utiliza aceasta pentru calculele
viitoare. Initial se aloca o matrice noua, identica. Apoi se 
parcurge matricea in functie de select si se verifica pixelii 
care au vecini (cei care nu se afla pe prima/ultima linie/coloana). 
Daca au destui vecini, se vor inmulti cei 9 pixeli(cel de la pasul
curent + 8 vecini) cu corespondentul din matricea kernel si se vor 
aduna intr-o variabila. Dupa ce se termina inmultirile se verifica
daca parametrul este de tip 1(BLUR), sau 3(GAUSSIAN_BLUR), si se 
va mai face o ultima operatie(/9 sau /16) asupra sumei.
Valoarea se va rotunji cu round si se va reduce in intervalul
[0,255] prin functia clamp si se copiaza in matricea noua.
Dupa parcurgerea matricei se elibereaza cea initiala si i se 
atribuie adresa celei noi.
10. Comanda EXIT 
Aceasta in cazul in care s-a incarcat o imagine va elibera 
spatiul acesteia, cat si spatiul dat de magic word care a 
fost alocat dinamic.