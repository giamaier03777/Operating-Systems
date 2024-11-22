#!/bin/bash


if [ "$#" -ne 1 ]; then #daca nr de arg <1, nu e datei
    echo "Datei fehlt!"
    exit 1
fi


filename="$1"


if [ ! -f "$filename" ]; then
    echo "Datei existiert nicht"
    exit 1
fi


for i in {1..5}; do
    line=$(sed -n "${i}p" "$filename")
 #sed pt a extrage linia i din file si o salveaza in variabila line
    if [ ${#line} -lt 10 ]; then  #verif daca lungimea liniei < 10 caractere        
echo "Jede Zeile soll mindestens 10 Charaktere haben"
        exit 1
    fi
    char=${line:i-1:1} #extragere elem
    echo -n "$char " #afis caract extras + spatiu
done

echo