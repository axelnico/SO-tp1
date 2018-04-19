option="none"
while getopts 'hst:' opt; do
	case $opt in
		h)	echo ""
			echo "    Script para generar info de experimentacion. Se calcula el tiempo de"
			echo "    reloj antes y despues de cada ejecucion, realizando 100 veces cada una."
			echo ""
			echo "    Opciones disponibles:"
			echo "        -h    		  Imprime este texto de ayuda."
			echo "        -s    		  Realiza la comparacion de a un thread"
			echo "        -t <#threads>   Realiza la comparacion solamente entre multithreds con un maximo de t threads"
			echo ""
			exit 0 ;;
		s) option=1 ;;
		t) option=$OPTARG ;;
	esac
done

if [[ $option = "none" ]]; then
	echo ""
	echo "    Script para generar info de experimentacion. Se calcula el tiempo de"
	echo "    reloj antes y despues de cada ejecucion, realizando 500 veces cada una."
	echo ""
	echo "    Opciones disponibles:"
	echo "        -h    		  Imprime este texto de ayuda."
	echo "        -t <#threads>   Realiza la comparacion solamente entre multithreds con un maximo de t threads"
	echo ""
	exit 0
fi

make clean -C ../entregable
make -C ../entregable

if [[ $option > 1 ]]; then
	echo "corriendo comparacion entre multithread hasta ($option) threads"
	cd ../entregable
	rm multi-*
	for ((t = 1; t <= $option; t=t+1)); do
		for (( i = 0; i < 500; i=i+1 )); do
			./max-compare $t 5 >> ../scripts/multi-$t
		done
	done
	cd ../scripts
fi

