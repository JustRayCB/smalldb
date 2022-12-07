#/usr/bin/bash

if ! [ -e ../smalldb ] ; then
    echo "Pas d'exécutable smalldb"
    exit 2
elif pidof -q smalldb ; then
    killall smalldb
fi
if ! [ -e ../sdbsh ] ; then
    echo "Pas d'exécutable sdbsh"
    exit 2
fi

idx=1
for f in queries/*.txt ; do
    echo -e "\n>>> copie la BDD"
    db="data/$(date -Ins).bin"
    file="${idx}.txt"
    cp data/test_db.bin "${db}"

    echo ">>> lance le serveur"
    ../smalldb "${db}" &
    if ! pidof -q smalldb ; then
        echo "Impossible de lancer le serveur"
        exit 1
    fi

    echo ">>> lance le client"
    ../sdbsh 127.0.0.1 < "${f}" > "data/${file}"

    echo ">>> tue le serveur"
    killall smalldb

    echo ">>> supprime la BDD temporaire"
    rm "${db}"

    echo ">>> compare les résultats"
    if ! diff -q "data/${file}" "${f%.txt}.result" ; then
        echo -e "\n\033[1;31mErreur avec $f\033[0m"
        diff --color -y  "data/${file}" "${f%.txt}.result"
    else
        echo -e "\033[1;32mOK\033[0m"
    fi
    idx=$((idx+1))
    sleep 2
done

if ! [ -e ../smalldbctl ] ; then
    echo "Pas d'exécutable smalldbctl"
    exit 3
fi
