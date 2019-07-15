
conf="conf/offline_ir.conf"
cur_date=20180918
task="ir"

end_date=20180918
done_file="ir.done"
day_interval=1

while [ $cur_date -le $end_date ]
do
    sh -x offline/run_zhaolei.sh "$conf" "$cur_date" "$task" >& log/offline_xfea.$task.$cur_date
    if [ $? -ne 0 ];then
        echo "Run [sh -x offline/run_zhaolei.sh $conf $cur_date $task] failed!"
        exit 1
    else
        echo "Run [sh -x offline/run_zhaolei.sh $conf $cur_date $task] successfully."
    fi
    echo "$cur_date `date \"+%Y-%m-%d %H:%M:%S\"`"  >> $done_file
    cur_date=`date -d "$cur_date $day_interval days" "+%Y%m%d"`
done

