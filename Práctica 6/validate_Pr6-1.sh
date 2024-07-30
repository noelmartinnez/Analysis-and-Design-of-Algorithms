#Análisis y diseño de algoritmos (II-I2ADE - DLSI-UA) 2023-24
#Validación del archivo de la práctica 6.1 que se pretende entregar

# BITS DEL CODIGO DE RETORNO: _ _ _ _
#                             | | | |
#                             | | | -> No se admite la entrega ( o fichero inexistente)
#                             | | -> No gestiona bien los errores en los argumentos
#                             | -> No hace mcp_memo()
#                             -> No hace mcp_naive() (en esta práctica, esto no se permite -entrega no válida-)

RETURN_CODE=0   
filename=$1
echo "Entrega de la práctica 6.1. Fichero a validar:" $filename

[   -z "$filename" ] && { echo $0: missing tar-gz-file; exit 1; }
[ ! -f "$filename" ] && { echo $0: file \'$filename\' not found; exit 1; }

timeout_1="timeout 0.5"

#Crear directorio tmp_dir
tmp_dir=$(mktemp -d /tmp/validate_dir.XXXXXX)

# Acciones al terminar
by_the_end() {
    exit_code=$?
    [[ $warnings  -ne 0 ]] && echo "ATENCIÓN: Los avisos mostrados mermarán considerablemente la nota."
    [[ $exit_code -ne 0 ]] && echo "VALIDACIÓN NEGATIVA: Debes corregir los errores antes de entregar." && RETURN_CODE=$((RETURN_CODE |= 1))
    if [[ $exit_code -eq 0 ]]; then
        echo
        echo "DNI/NIE asociado al archivo comprimido: "$dni
        echo "Nombre capturado en la primera línea de mcp.cc: "$name
        echo "Asegúrate de que la captura de DNI/NIE y nombre es correcta."
        echo
        echo "VALIDACIÓN POSITIVA: El trabajo cumple las especificaciones, pero asegúrate de que también compila en los ordenadores del aula."
    fi
    rm -r $tmp_dir
    exit $RETURN_CODE
}

check_err_args() {
    error_message=$($1 2>&1 1>/dev/null)
    ret=$? # 124=timeout; 134=seg-fault; 139=core-dumped
    if [[ ret -eq 124 ]] || [[ ret -eq 134 ]] || [[ ret -eq 139 ]]; then
        echo "Aviso: '$2' produce violación de segmento o se cuelga."
        warnings=1
        RETURN_CODE=$(( RETURN_CODE |= 2 ))
    fi
    if [[ -z "$error_message" ]]; then
        echo "Aviso: '$2' no emite mensaje de error (por la salida de error)."
        warnings=1
        RETURN_CODE=$(( RETURN_CODE |= 2 ))
    fi 
}

check_code() {
    gprof -p -b $1 gmon.out | grep $2
    if [ $? -ne 0 ]; then
        check_call_funct $2
    fi
    check_memory_map --save $1 $2    
}

launch() {
    captured=$($1); ret=$?
    case $ret in
	    124) msg_timeout="tarda demasiado.";;
	    134) msg_timeout="produce violación de segmento.";;
	    139) msg_timeout="produce un 'core'.";;
	    *)   msg_timeout="";ret=0;;
    esac
    if  [[ $ret -ne 0 ]]; then
        echo "Error: la orden "$msg_timeout
        echo "prueba realizada (num. validación=$3):" $2
        exit 1
    fi 
    return $ret
}

#captura señales
trap by_the_end EXIT SIGINT SIGTERM


# Trabajar en el directorio temporal
cp "$filename" $tmp_dir
cd $tmp_dir
filename=${filename##*/}
warnings=0
name="<empty>"
captured=""
msg_timeout=""
check_num=0
#check_num=$(echo $check_num + 1 | bc)


# dni/nie y extensión del archivo que se pretende entregar
check_num=1
dni=$(echo "$filename" | cut -d "." -f 1)
ext=.$(echo "$filename" | cut -d "." -f 2-3) # | tr -d '\n' | tail -c 2)
allowed_ext=".tar.gz .tgz"
if [[ ! " $allowed_ext " =~ " $ext " ]];then
    echo "Error: Las extensiones permitidas son: {"$allowed_ext"}"
    echo "extensión capturada:" $ext
    exit 1
fi

# desempaquetar
check_num=2
tar -xzvf "$filename" > /dev/null
if [ $? -ne 0 ]; then
    echo "Error: 'tar' falló al desempaquetar "$filename
    exit 1
fi
rm "$filename"


# Verificar que están todos los archivos que se pide
check_num=3
must_be_files="mcp.cc [mM]akefile"
for f in ${must_be_files}; do
    if [ ! -f $f ]; then
        echo "Error: El archivo $f no está."
        exit 1
    fi 
done


# Verificar que solo se entrega lo que se pide
check_num=4
allowed_files="mcp.cc makefile Makefile"
for f in *; do
    if [[ ! " $allowed_files " =~ " $f " ]];then 
        echo "Error: Se pretende entregar un archivo no permitido: $f"
        exit 1
    fi
done

# Verificar la primera línea del archivo 
check_num=5
name=$(cat mcp.cc | head -n 1)
if [[ ! "$name" =~ ^"//" ]] && [[ ! "$name" =~ ^"/*" ]]; then
    echo "Error: La primera línea de mcp.cc no contiene tu nombre."
    exit 1
fi 


#compilacion
check_num=6
make > /dev/null
if [ $? -ne 0 ]; then
    echo "Error: 'make' ha devuelto error."
    exit 1
fi


# Verificar si el archivo ejecutable "mcp" existe
check_num=7
if [ ! -x "mcp" ]; then
    echo "Error: El archivo 'mcp' no existe o no es ejecutable."
    exit 1
fi


# Comprobar que esta definida la funcion mcp_memo
check_num=8
grep -q mcp_memo mcp.cc
if [ $? -ne 0 ]; then
    echo "Aviso: mcp_memo() no encontrada; se asumirá que no ha sido implementada."
    warnings=1
    RETURN_CODE=$(( RETURN_CODE |= 4 ))
fi 


# Comprobar que esta definida la funcion mcp_naive
check_num=9
grep -q mcp_naive mcp.cc
if [ $? -ne 0 ]; then
    echo "ERROR: mcp_naive() no encontrada, pero es obligatoria."
    RETURN_CODE=$(( RETURN_CODE |= 8 ))
    exit 1
fi 


# Creando los mapas de prueba
test_1=test1.map  #en realidad es 02.map
cat << 'EOF' > $test_1
6 5
1 3 5 1 1
2 4 6 3 1
1 2 9 7 1
9 1 7 1 9
1 3 7 5 1
8 1 2 2 1
EOF

cat << 'EOF' > ${test_1}.path
x....
x....
x....
.x...
.x...
..xxx
13
EOF

test_2=test2.map #en realidad es 01.map con la dificultad de la celda cambiada
cat << 'EOF' > $test_2
1 1
5
EOF

# Comprobaciones en la línea de comandos. 
# Ante cualquier aviso, se considerará que la gestión de argumentos no es robusta
check_num=11
command="$timeout_1 mcp -f"
command2show="mcp -f"
check_err_args "$command" "$command2show"

check_num=12
command="$timeout_1 mcp -f $test_1 -f"
command2show="mcp -f 02.map -f"
check_err_args "$command" "$command2show"

check_num=13
command="$timeout_1 mcp"
command2show="mcp"
check_err_args "$command" "$command2show"

check_num=14
command="$timeout_1 mcp -f -// "
command2show="mcp  -f -//"
check_err_args "$command" "$command2show"

check_num=15
command="$timeout_1 mcp  -f $test_1 -t -mal1 -mal2"
command2show="mcp -f 02.map -t -mal1 -mal2"
check_err_args "$command" "$command2show"

check_num=16
command="$timeout_1 mcp -f $test_1 -f inexistentefile -t"
command2show="mcp -f 02.map -f inexistentefile -t"
check_err_args "$command" "$command2show"



# Comprobaciones en la solución
check_num=17
command="$timeout_1 mcp -f $test_1"
command2show="mcp -f 02.map"
launch "$command" "$command2show" $check_num
if [[ ! $(echo $captured | grep -Eo '13|\?' | wc -l) -eq 4  ]]; then
    echo "Error: salida (o su formato) incorrecta en la prueba '$command2show'"
    echo "salida capturada (num. validación=$check_num):"
    echo "$captured"
    exit 1
fi


check_num=18
command="$timeout_1 mcp -f $test_1"
command2show="mcp -f 02.map"
launch "$command" "$command2show" $check_num
if [[ ! $(echo $captured | wc -w) -eq 4  ]]; then
    echo "Error: formato de salida incorrecto en la prueba '$command2show'"
    echo "salida capturada (num. validación=$check_num):"
    echo "$captured"
    exit 1
fi


check_num=19
command="$timeout_1 mcp --ignore-naive -f $test_1"
command2show="mcp --ignore-naive -f 02.map"
launch "$command" "$command2show" $check_num
if [[ ! "$captured" =~ ^[[:space:]]*-[[:space:]] ]]; then
    echo "Error: salida (o su formato) incorrecta en la prueba '$command2show'"
    echo "salida capturada (num. validación=$check_num):"
    echo "$captured"
    exit 1
fi


check_num=20
command="$timeout_1 mcp -f $test_2 -t"
command2show="mcp -f 01.map -t"
launch "$command" "$command2show" $check_num
if [[ ! $(echo $captured | grep -Eo '5|\?' | wc -l) -eq 5  ]]; then
    echo "Error: salida (o su formato) incorrecta en la prueba '$command2show'"
    echo "salida capturada (num. validación=$check_num):"
    echo "$captured"
    exit 1
fi

param_path="--p2D"
msg_bad_output_path="En la posición donde se debe mostrar el camino, se espera el carácter '?' o el camino correcto (en el formato correcto)"

check_num=21
command="$timeout_1 mcp $param_path -f $test_2 --ignore-naive"
command2show="mcp $param_path -f 01.map --ignore-naive"
launch "$command" "$command2show" $check_num
captured=$(echo "$captured" | head -n 3)
num_l=$(echo "$captured" | wc -l)
chars=$([[ $num_l  -eq 3 ]] && echo 2 || echo 1)
select=$(echo $captured | tr -d ' \n' | tail -c $chars)
case $num_l in
    2) [[ "$select" == "?" ]]   && error=0 || error=1;;
    3) [[ "$select" == "x5" ]] && error=0 || error=1;;
    *) error=1;;
esac
if [[ $error -eq 1 ]]; then
    echo "Error: salida (o su formato) incorrecta en la prueba '$command2show'"
    echo "salida capturada (num. validación=$check_num):"
    echo "$captured"
    echo $msg_bad_output_path
    exit 1
fi


check_num=22
command="$timeout_1 mcp $param_path -f $test_2 --ignore-naive -t"
command2show="mcp $param_path -f 01.map --ignore-naive -t"
launch "$command" "$command2show" $check_num
captured=$(echo "$captured"  | tail -n +2 | head -n 3 )
error=1
for i in x5? x55 ?? ?5; do   #secuencias válidas
   [[ "$(echo $captured | tr -d ' ')" == "$i" ]] && { error=0; break; }
done
if [[ $error -eq 1 ]]; then
    echo "Error: salida (o su formato) incorrecta en la prueba '$command2show'"
    echo "salida capturada (num. validación=$check_num):"
    echo "$captured"
    echo $msg_bad_output_path
    exit 1
fi

check_num=23
command="$timeout_1 mcp $param_path -f $test_1 --ignore-naive"
command2show="mcp $param_path -f 02.map --ignore-naive"
launch "$command" "$command2show" $check_num
captured=$(echo "$captured"  | tail -n +2 | head -n 7 )
if [[ $(echo $captured | tr -d ' ') != "?" ]] && [[ $captured != "$(cat ${test_1}.path)" ]]; then
    echo "Error: salida (o su formato) incorrecta en la prueba '$command2show'"
    echo "salida capturada (num. validación=$check_num):"
    echo "$captured"
    echo $msg_bad_output_path
    exit 1
fi

exit 0

