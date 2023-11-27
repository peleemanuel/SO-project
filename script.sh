# intrarea linie cu linie de la tastatura
# facem filtrarea cu un regex din program
# daca respecta reafisam altfel scriem incorect
# dupa ce am afisat citim in continuare o sa astepte linii de la tastaura
# ce trebuie sa respectam ce a avut ora trecuta ^[A-Z][a-z0-9, ]*\.$, sa respecte regulile unei propozitii  
#intre cuvantul "si" si cuvantul anterior sa nu fie virgula
#incainte de p si b trebuie sa fie m nu n
# punctul exista o singura data in propozitie
#!/bin/bash

if test $# -lt 1
then
    echo "Prea putine argumente"
    exit 1
elif test $# -gt 1
then
    echo "Prea multe argumente"
    exit 1
fi

caracter=$1
contor=0

while read x
do
    aux1=`echo "$x" | grep -E "^[A-Z][a-z0-9, -]*(\.|\?|\!)$" | grep -vE ',[ ]*si'  | grep -vE 'si[ ]*,'`
    if test -n "$aux1"; then
        if [[ $aux1 == *"$caracter"* ]];then
            contor=`expr $contor + 1`
        fi
    fi

done

echo "$contor"
