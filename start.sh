OFF='\033[0m'
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

Port=$1

if [ -z "$Port" ]; then
	echo -e "❌$Red Aucun port n'a été spécifié$OFF : ./start.sh <port>"
	exit
fi

(echo -e > /dev/tcp/127.0.0.1/$port) >/dev/null 2>&1
if [ ! $? -ne 0 ]; then
	echo "$Yellow Le port $port est peut-être déjà utilisé ou invalide.$OFF"
fi

echo -e -n "🔨 Compilation du testeur :"
gcc tester.c -o tester

if [ ! -f "tester" ]; then
	echo -e " ❌$Red Le testeur n'a pas pu être compilé.$OFF"
	exit
else
	echo -e "$Green OK$OFF"
fi

echo -e -n "🔨 Compilation du mini_serv :"
gcc ../mini_serv.c -o mini_serv

if [ ! -f "mini_serv" ]; then
	echo -e " ❌$Red Le mini_serv n'a pas pu être compilé.$OFF"
	exit
else
	echo -e "$Green OK$OFF"
fi

echo -e -n "🚀 Lancement du testeur :"

execoutput=$(./tester $Port)

if [[ "$execoutput" =~ .*"ERROR ARGS".* ]]; then
	echo -e " ❌$Red Erreur d'arguments dans le testeur.$OFF"
	exit
fi

if [[ "$execoutput" =~ .*"ERROR FILE".* ]]; then
	echo -e " ❌$Red Le testeur n'a pas pu créer le fichier de sortie.$OFF"
	exit
fi

if [[ "$execoutput" =~ .*"ERROR CONNECT".* ]]; then
	echo -e " ❌$Red Le testeur n'a pas pu se connecter au mini_serv.$OFF"
	echo -e "❓ Veuillez vérifier que le port $Port est bien disponible, retentez avec un port différent."
	exit
fi

if [[ "$execoutput" =~ .*"OK".* ]]; then
	echo -e "$Green OK$OFF"
fi

fichier1="output.txt"
fichier2="desired_output.txt"

if [ ! -f "$fichier1" ]; then
	echo -e "❌$Red Le fichier de sortie n'a pas été créé.$OFF"
	exit
fi

if [ ! -f "$fichier2" ]; then
	echo -e "❌$Red Le fichier de sortie désiré est introuvable.$OFF"
	exit
fi

echo -e -n "🔨 Comparaison des fichiers de sortie :"

if cmp -s "$fichier1" "$fichier2"; then
    echo -e "$Green OK$OFF"
	echo -e "🎉$Green Tous les tests ont été passés avec succès.$OFF"
else
    echo -e "$Red KO$OFF"
	echo -e "❌$Red Les fichiers de sortie ne sont pas identiques.$OFF\n"
	echo -e "❓ Pour plus d'informations, veuillez consulter les fichier $fichier1 et $fichier2."
	echo -e "❓ Le serveur devra tout de même afficher les messages des clients se deconnectant pendant l'envoi."
fi
