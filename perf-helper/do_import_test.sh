#!/bin/bash
filename="$1"
pid="$(pidof arangod)"
collection="${2:-ulf}"
port="${3:-6666}"

page_size_bytes="$(getconf PAGESIZE)" 
page_size_KiB="$((page_size_bytes / 1024))" 

get_meminfo_top(){
    local line="$(top -p $1 -n1 -b | tail -n1 )"
    local VIRT="$( awk '{print $5}' <<< "$line" )"
    local RES="$( awk '{print $6}' <<< "$line" )"
    local SHR="$( awk '{print $7}' <<< "$line" )"
    echo "VIRT $VIRT RES $RES SHR $SHR (no fixed units)"
}

get_meminfo_ps(){
    local line="$(ps -p$1 -o rss,vsz | tail -n1)"
    local RSS="$( awk '{print $1}' <<< "$line" )"
    local VSZ="$( awk '{print $2}' <<< "$line" )"
    echo "VSZ_KiB $VSZ RSS_kB $RSS - KiB though manpage claims other wise"
}

get_meminfo_statm(){
    local size="$( awk '{print $1}' < /proc/$1/statm )"
    local resident="$( awk '{print $2}' < /proc/$1/statm )"
    local shared="$( awk '{print $3}' < /proc/$1/statm )"
    echo "VmSize $((size * page_size_KiB)) VmRss $((resident * page_size_KiB)) RssFile+RssShmem $((shared * page_size_KiB)) - KiB"
}

get_meminfo_stat(){
    local vsize="$( awk '{print $23}' < /proc/$1/stat )"
    local rss="$( awk '{print $24}' < /proc/$1/stat )"
    echo "vsize $(( vsize / 1024 )) rss $(( rss * page_size_KiB )) - KiB"
}

get_meminfo_status(){
	local VmSize="$(grep VmSize /proc/$1/status | awk '{print $2}'  )"
	local VmRSS="$(grep VmRSS /proc/$1/status | awk '{print $2}'  )"
	echo "VmSize $VmSize VmRSS $VmRSS- KiB"
}

run_import_test(){
    local arango_pid="$1"
    local filename="$2"
    local collection_name="$3"
    local port="$4"

    echo "using file name: $filename"
    echo "inspectiong arangod with pid: $arango_pid"

    start_time="$(date +%s)"
    while read line <&4; do
        echo "$line"
        case $line in
            *processed*) #this marker is written by arango import every 3%
                local percent="$(echo $line | awk '{print $7}' | tr -d '(%)')"
                #http://man7.org/linux/man-pages/man5/proc.5.html
                # statm 1(size) 2(resident) 3(shared)
                # meminfo="$(cat /proc/$arango_pid/statm | awk '{print "size "$1" resident "$2" shared "$3}')"
                local time_percent="percent $percent time_s $(($(date +%s) - start_time))"
                local meminfo="$(get_meminfo_statm $arango_pid)"
                echo "$time_percent $meminfo" | tee -a import.log
            ;;
        esac
    done 4< <(./build/bin/arangoimp --type csv --server.endpoint http+tcp://localhost:$port --collection "$collection_name" --create-collection true "$filename")
    end_time="$(date +%s)"
    duration=$((end_time - start_time))
    echo "the process took $duration seconds"
}

echo "running: run_import_test $pid $filename $collection $port"
run_import_test "$pid" "$filename" "$collection" "$port"

#test methods to aquire memory information
#pid=$(pidof man)
#get_meminfo_top $pid
#get_meminfo_ps $pid
#get_meminfo_statm $pid
#get_meminfo_stat $pid
#get_meminfo_status $pid
