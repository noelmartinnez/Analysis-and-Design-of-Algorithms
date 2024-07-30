#!/bin/bash

#Análisis y diseño de algoritmos (II-I2ADE - DLSI-UA) 2023-24
#Validación del archivo de la práctica 6.2 que se pretende entregar
#       USAGE: $0 tar-gz-file [-h] [--quiet] [-d dir_home]

# BITS DEL CODIGO DE RETORNO: _ _ _ _ _
#                             | | | | |
#                             | | | | -> No se admite la entrega ( o fichero inexistente)
#                             | | | -> No gestiona bien los errores en los argumentos
#                             | | -> No hace mcp_it_matrix(): no se realizará captura para su valoracion
#                             |  -> No hace mcp_it_vector(): no se realizará captura para su valoracion 
#                             -> No hace mcp_parser(): no se realizará captura para su valoracion
export PATH=.:$PATH

#ATENCIÓN USUARIOS DE MAC:
#puede que no tengas la orden 'timeout' en el sistema (lo puedes saber escribiendo 'timeout' en la terminal);
#en tal caso, deja la siguiente variable vacía, es decir: timeout_1=""
#pero ten en cuenta que ya NO se detectará si tu código se cuelga o si produce una violación de segmento
#tendrás que asegurarte tú mism@ de que eso no ocurre (al igual que con la compilación en MAC)
timeout_1="timeout 0.5"
#timeout_1=""  #para el caso de no disponer de la orden 'timeout'

# Variables to assign in each practice
source="mcp.cc"
exe="mcp"
err_file="errors_pr62.log"
warn_file="warnigs_pr62.log"
must_be_files="${source} [mM]akefile"
allowed_files="${source} makefile Makefile ${err_file} ${warn_file}"
should_be_func=(mcp_it_matrix mcp_it_vector mcp_parser)

#Control variables
RETURN_CODE=0
allowed_ext=".tar.gz .tgz"
warnings=0
name="<unknown>"
captured=""
msg_timeout=""
check_num=0

filename=""
dir_home="" #not used in validate-mode
dir_test="" #not used in validate-mode
quiet=0

#argument management
args=( "$@" )
for ((i=0; i<${#args[@]}; i++)); do
    is_filename=1
    [[ ${args[$i]} == "--quiet" ]] && { quiet=1; is_filename=0; }
    [[ ${args[$i]} == "-d" ]] && { i=$((i+1)); dir_home=${args[$i]}; is_filename=0; }
    [[ ${args[$i]} == "-h" ]] && { echo "usage: $0 tar-gz-file [-h] [--quiet] [-d dir_home]";  exit 255; }
    [[ is_filename -eq 1 ]] && filename="${args[$i]}"
done

[[ $quiet -eq 0 ]] && echo "Entrega de la práctica 6.2"
[[ $quiet -eq 0 ]] && echo "Fichero a validar: ${filename}"

[   -z "$filename" ] && { echo $0: missing tar-gz-file; exit 255; }
[ ! -f "$filename" ] && { echo $0: file \'$filename\' not found; exit 255; }



#Create tmp directory
tmp_dir=$(mktemp -d /tmp/validate_dir.XXXXXX)

# Actions at the end
by_the_end() {
    exit_code=$?
    [[ -f ${err_file} ]] && exit_code=1
    [[ -f ${warn_file} ]] && warnings=1
    [[ $exit_code -ne 0 ]] && RETURN_CODE=$((RETURN_CODE |= 1))
    if [[ $quiet -eq 0 ]]; then
        [[ -f ${warn_file} ]] && { echo; cat ${warn_file}; }
        [[ $warnings  -ne 0 ]] && echo -e "\nATENCIÓN: Los avisos mostrados mermarán considerablemente la nota."
        [[ -f ${err_file} ]] && { echo; cat ${err_file}; }
        if [[ $exit_code -eq 0 ]]; then
            echo -e "\nDNI/NIE asociado al archivo comprimido: "$dni
            echo -e "Nombre capturado en la primera línea de ${source}: "$name
            echo -e "Asegúrate de que la captura de DNI/NIE y nombre es correcta."
            echo -e "\nVALIDACIÓN POSITIVA: El trabajo cumple las especificaciones, pero asegúrate de que también compila en los ordenadores del aula."
        else
            echo -e "\nVALIDACIÓN NEGATIVA: Debes corregir los errores antes de entregar."
        fi
#    else
#        echo -n $RETURN_CODE
    fi
    rm -r $tmp_dir
    exit $RETURN_CODE
}


check_err_args() {
    error_message=$($1 2>&1 1>/dev/null)
    ret=$? # 124=timeout; 134=seg-fault; 139=core-dumped
    if [[ ret -eq 124 ]] || [[ ret -eq 134 ]] || [[ ret -eq 139 ]]; then
        echo "Aviso: '$2' produce violación de segmento o 'timeout'" >> ${warn_file}
        warnings=1
        RETURN_CODE=$(( RETURN_CODE |= 2 ))
    fi
    if [[ -z "$error_message" ]]; then
        echo "Aviso: '$2' no emite mensaje de error (por la salida de error)"  >> ${warn_file}
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

error_msg(){
    echo -e "\n*** Validación número $3 ***" >> ${err_file}
    echo -e "  Prueba realizada: $2" >> ${err_file}
    echo -e "  $1" >> ${err_file}
    RETURN_CODE=$(( RETURN_CODE |= 1 ))
}

launch() {
    [[ -f tmp_file ]] && rm tmp_file
    captured=$($1 2>tmp_file); ret=$?
    case $ret in
	    124) msg_timeout="tarda demasiado ('timeout').";;
	    134) msg_timeout="produce violación de segmento.";;
	    139) msg_timeout="produce un 'core'.";;
        *)   [[ -s tmp_file ]] && { msg_timeout="no debe mostrar nada por la salida de error puesto que es correcta."; ret=1; } || ret=0
    esac
    [[ $ret -ne 0 ]] && error_msg "Error: la orden ${msg_timeout}" "$2" $3
    return $ret
}

#capture signals
trap by_the_end EXIT SIGINT SIGTERM


# Work in tmp dir
cp "$filename" $tmp_dir
cd $tmp_dir
filename=${filename##*/}



# dni/nie & extension of the file to be delivered
check_num=1
dni=$(echo "$filename" | cut -d "." -f 1)
ext=.$(echo "$filename" | cut -d "." -f 2-3) # | tr -d '\n' | tail -c 2)
if [[ ! " $allowed_ext " =~ " $ext " ]];then
    echo "Error: Las extensiones permitidas son: {"$allowed_ext"}."  >> ${err_file}
    echo -e "\t(extensión capturada: $ext )" >> ${err_file}
    RETURN_CODE=$((RETURN_CODE |= 1))
    exit 1
fi

# unpack tgz
check_num=2
tar -xzvf "$filename" > /dev/null
if [ $? -ne 0 ]; then
    echo "Error: 'tar' falló al desempaquetar ${filename}." >> ${err_file}
    RETURN_CODE=$((RETURN_CODE |= 1))
    exit 1
fi

rm "$filename"
[[ -f ${err_file}  ]] && rm ${err_file}
[[ -f ${warn_file} ]] && rm ${warn_file}


# Verify that only what is requested is delivered
check_num=3
missing=""
for f in ${must_be_files}; do
    if [ ! -f $f ]; then
        missing=${missing}" "${f}
    fi 
done
if [[ ! -z ${missing} ]]; then
    echo "Error: Falta/n archivo/s en la entrega (${missing} )." >> ${err_file}
    RETURN_CODE=$((RETURN_CODE |= 1))
fi


# verify that only what is necessary is delivered
check_num=4
unwanted=""
for f in *; do
    if [[ ! " $allowed_files " =~ " $f " ]];then 
        unwanted=${unwanted}" "${f}
    fi
done
if [[ ! -z ${unwanted} ]]; then
    echo "Error: Se entregan más archivos de los permitidos (${unwanted} )." >> ${err_file}
    RETURN_CODE=$((RETURN_CODE |= 1))
fi


#name="$(echo -e "${name}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')" # All-TRIM
#tr -d "[:space:]" #elimina tb saltos de linea, a diferencia de tr -d "" que elimina solo espacios
# Verify the student's name in the first line of the source (only on that line)
check_num=5
[[ ! -f ${source} ]] && { RETURN_CODE=$((RETURN_CODE |= 1)); exit 1; }
name=$(cat ${source} | head -n 1 | tr '*' '/' | xargs)
[[ "$name" =~ ^"//" ]] && name=$(echo ${name} | tr -d '/' | xargs) || name="<unknown>"
[[ -z ${name} ]] && name="<unknown>"
if [[ "$name" == "<unknown>" ]]; then
    echo "Error: No se captura tu nombre en la primera línea del archivo ${source}." >> ${err_file}
    RETURN_CODE=$((RETURN_CODE |= 1))
fi

#compilation
check_num=6
make > /dev/null
if [ $? -ne 0 ]; then
    echo "Error: 'make' ha devuelto error." >> ${err_file}
    RETURN_CODE=$((RETURN_CODE |= 1))
    exit 1
fi


# Verify that the executable file exists
check_num=7
if [ ! -x "$exe" ]; then
    echo "Error: El archivo ${exe} no existe o no es ejecutable." >> ${err_file}
    RETURN_CODE=$((RETURN_CODE |= 1))
    exit 1
fi


# Check that the requested functions are defined
check_num=8
for ((i=0; i<${#should_be_func[@]}; i++)); do
    grep -q ${should_be_func[$i]} $source
    if [ $? -ne 0 ]; then    
     warnings=1
     RETURN_CODE=$((RETURN_CODE |= $(echo 2^$((i+2))|bc)))
     echo "Aviso: ${should_be_func[$i]} no encontrada; se asume que no se ha implementado."  >> ${warn_file}
    fi
done 

# creating test-maps
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

# Command line checks. 
# Ante cualquier aviso, se considerará que la gestión de argumentos no es robusta
check_num=11
command="$timeout_1 ${exe} -f"
command2show="${exe} -f"
check_err_args "$command" "$command2show"

check_num=12
command="$timeout_1 ${exe} -f $test_1 -f"
command2show="${exe} -f 02.map -f"
check_err_args "$command" "$command2show"

check_num=13
command="$timeout_1 ${exe}"
command2show="${exe}"
check_err_args "$command" "$command2show"

check_num=14
command="$timeout_1 ${exe} -f -// "
command2show="${exe}  -f -//"
check_err_args "$command" "$command2show"

check_num=15
command="$timeout_1 ${exe}  -f $test_1 -t -mal1 -mal2"
command2show="${exe} -f 02.map -t -mal1 -mal2"
check_err_args "$command" "$command2show"

check_num=16
command="$timeout_1 ${exe} -f $test_1 -f inexistentefile -t"
command2show="${exe} -f 02.map -f inexistentefile -t"
check_err_args "$command" "$command2show"



# Comprobaciones en la solución
check_num=17
command="$timeout_1 ${exe} -f $test_1"
command2show="${exe} -f 02.map"
launch "$command" "$command2show" $check_num
if [[ ! $(echo $captured | grep -Eo '13|\?' | wc -l) -eq 4  ]]; then
    [[ -z "$captured" ]] && captured="<void>"
    error_msg "salida capturada:\n$captured" "$command2show" "$check_num (ERROR)"
fi


check_num=18
command="$timeout_1 ${exe} -f $test_1"
command2show="${exe} -f 02.map"
launch "$command" "$command2show" $check_num
if [[ ! $(echo $captured | wc -w) -eq 4  ]]; then
    [[ -z "$captured" ]] && captured="<void>"
    error_msg "salida capturada:\n$captured" "$command2show" "$check_num (ERROR)"
fi


check_num=19
command="$timeout_1 ${exe} --ignore-naive -f $test_1"
command2show="${exe} --ignore-naive -f 02.map"
launch "$command" "$command2show" $check_num
if [[ ! "$captured" =~ ^[[:space:]]*-[[:space:]] ]]; then
    [[ -z "$captured" ]] && captured="<void>"
    error_msg "salida capturada:\n$captured" "$command2show" "$check_num (ERROR)"
fi


check_num=20
command="$timeout_1 ${exe} -f $test_2 -t"
command2show="${exe} -f 01.map -t"
launch "$command" "$command2show" $check_num
if [[ ! $(echo $captured | grep -Eo '5|\?' | wc -l) -eq 5  ]]; then
    [[ -z "$captured" ]] && captured="<void>"
    error_msg "salida capturada:\n$captured" "$command2show" "$check_num (ERROR)"
fi

param_path="--p2D"
msg_bad_output_path="  En la posición del camino, se espera el carácter '?' o el camino correcto (en el formato correcto)"

check_num=21
command="$timeout_1 ${exe} $param_path -f $test_2 --ignore-naive"
command2show="${exe} $param_path -f 01.map --ignore-naive"
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
    [[ -z "$captured" ]] && captured="<void>"
    error_msg "salida capturada:\n$captured \n $msg_bad_output_path" "$command2show" "$check_num (ERROR)"
fi

check_num=22
command="$timeout_1 ${exe} $param_path -f $test_2 --ignore-naive -t"
command2show="${exe} $param_path -f 01.map --ignore-naive -t"
launch "$command" "$command2show" $check_num
captured=$(echo "$captured"  | tail -n +2 | head -n 3 )
error=1
for i in x5? x55 ?? ?5; do   #secuencias válidas
   [[ "$(echo $captured | tr -d ' ')" == "$i" ]] && { error=0; break; }
done
if [[ $error -eq 1 ]]; then
    [[ -z "$captured" ]] && captured="<void>"
    error_msg "salida capturada:\n$captured \n $msg_bad_output_path" "$command2show" "$check_num (ERROR)"
fi

check_num=23
command="$timeout_1 ${exe} $param_path -f $test_1 --ignore-naive"
command2show="${exe} $param_path -f 02.map --ignore-naive"
launch "$command" "$command2show" $check_num
captured=$(echo "$captured"  | tail -n +2 | head -n 7 )
if [[ $(echo $captured | tr -d ' ') != "?" ]] && [[ $captured != "$(cat ${test_1}.path)" ]]; then
    [[ -z "$captured" ]] && captured="<void>"
    error_msg "salida capturada:\n$captured \n $msg_bad_output_path" "$command2show" "$check_num (ERROR)"
fi

( [[ -f ${err_file} ]] || [[ $((RETURN_CODE&1)) -eq 1 ]] ) && exit 1;
exit 0
#

