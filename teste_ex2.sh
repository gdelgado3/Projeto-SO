#!/bin/bash
# --- aplicações das VMs ---
printf '#!/bin/sh\necho "  [VM curta $$] a correr"\nsleep 2\necho "  [VM curta $$] acabou"\n' > /tmp/app_curta.sh
printf '#!/bin/sh\necho "  [VM longa $$] a correr"\nsleep 4\necho "  [VM longa $$] acabou"\n' > /tmp/app_longa.sh
chmod +x /tmp/app_curta.sh /tmp/app_longa.sh

# --- pasta de inputs ---
rm -rf /tmp/v1-inputs /tmp/CloudIST
mkdir -p /tmp/v1-inputs/sub
echo ola > /tmp/v1-inputs/dados.txt
echo sub > /tmp/v1-inputs/sub/ficheiro.txt

# --- .conf ---
rm -rf testes_ex2 && mkdir testes_ex2
printf 'D V1 /tmp/v1-inputs /tmp/app_curta.sh 2 50 4\nD V2 /tmp/v1-inputs /tmp/app_longa.sh 1 10 2\nR R1 V1 2 1\nA R1\nL\nE 3000\nL\n' > testes_ex2/a.conf
printf 'R R2 V2 1 2\nA R2\nL\n' > testes_ex2/b.conf
printf 'R R3 V1 1 2\nL\n' > testes_ex2/c.conf

# --- correr ---
make && time ./cloudIST 2 16 500 8 testes_ex2

# --- verificações ---
echo; echo "== Zombies (deve estar vazio) =="
ps -eo stat,pid,comm | awk '$1 ~ /Z/'
echo "== Pastas das VMs =="
find /tmp/CloudIST | sort