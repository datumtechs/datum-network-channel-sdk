if [ $# -lt 2 ];
then
    echo "[command] [bytes] [record_count]"
    exit
fi
./p0_cs $1 $2 &
./p1_cs $1 $2 &
./p2_cs $1 $2 
