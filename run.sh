###########################################################################
#########  请先执行./offline里头的编译脚本编译好必须的可执行文件  #########
#########   cd offline; sh build.sh                               ######### 
###########################################################################
#cd offline; sh build.sh

mkdir -p ./log
PERSON=hongqing
#离线框架的配置
off_conf="offline/conf/offline.conf"
#由于配置项可能很多，不能都作为参数出入，所以这里做如下限制：
# 1. 配置都放到一个目录下，为了确保线上线下一致，最好都放到bisheng的conf目录下
# 2. feature_list,以及以后一堆ctr_map_txt的配置从bisheng_ir_off.conf里头读取
# 3. 建议不同的产品线用不同的名字
bisheng_conf_dir="bisheng/conf/"
bisheng_root_conf="bisheng_ir_off.conf"

cur_date=20181007
task="ir"

end_date=20181007
done_file="tmp.done"
day_interval=1

base_op_dir=/user_ext/ads_fst/${PERSON}/xfea/

##    columns: "label,ages,age,gender,platform,phone,location,network,hour,bidtype,style_type,psid,hierarchy_smooth_ctr,history_ctr,gender_feed_smooth_ctr,gender_cust_smooth_ctr,platform_feed_smooth_ctr,platform_cust_smooth_ctr,cust60_smooth_ctr,uid,custid,adid,feedid,promotion,position,style,link,show,zerocate,fircate,seccate"
## 如果hive_table这个选项有的话，则自动获取hive表的schema
## hive_table=sds_ad_algo_sfst_train_data2
while [ $cur_date -ge $end_date ]
do
    input_dir=${base_op_dir}/${cur_date}/input
    #有些hive表是以rcfile格式存储，改写到能以行为单位解析的格式
    hive_sql_before_run_fe_extract="select if(split(ie_cntx,',')[2]='0','0','1') as irl,age_s as ages,age,gender,platform,phone_brand as phone,location,network_type as network,pv_hour as hour,bid_type as bidtype,style_type,psid,hierarchy_smooth_ctr,history_ctr,gender_feed_smooth_ctr,gender_cust_smooth_ctr,platform_feed_smooth_ctr,platform_cust_smooth_ctr,cust60_smooth_ctr,uid,custid,adid,feedid,promotion_object as promotion,position,style,link,show,concat(substr(first_code,0,2),'000000') as zerocate,if(first_code is null,'0',first_code) as fircate,if(second_code is null,'0',second_code) as seccate from sds_ad_algo_sfst_train_data2 where dt='${cur_date}'"
    tmp_sql="insert overwrite directory '${input_dir}'
                ${hive_sql_before_run_fe_extract}"
    hive -e "$tmp_sql"
    if [ $? -ne 0 ]; then
        echo "run hive [$tmp_sql] failed!"
        exit 1
    fi
    sh -x offline/run.sh "$cur_date" ${base_op_dir} $task $off_conf $bisheng_conf_dir $bisheng_root_conf >& log/offline_xfea.$task.$cur_date
    if [ $? -ne 0 ];then
        echo "Run [sh -x offline/run.sh "$cur_date" ${base_op_dir} $task $off_conf $bisheng_conf_dir $bisheng_root_conf] failed!"
        exit 1
    else
        echo "Run [sh -x offline/run.sh "$cur_date" ${base_op_dir} $task $off_conf $bisheng_conf_dir $bisheng_root_conf] successfully."
    fi
    echo "$cur_date `date \"+%Y-%m-%d %H:%M:%S\"`"  >> $done_file
    cur_date=`date -d "$cur_date -$day_interval days" "+%Y%m%d"`
done
