#!/usr/bin/bash
#############################################
# Author: xfea-team#
# # # Date: 2018-08-16 #
# # # Describe: # 模板 # 
# # #############################################
# # ############# Config init ...... ############
export WK_DIR=`pwd`/offline
source /usr/local/jobclient/config/.hive_config.sh
source /usr/local/jobclient/lib/source $0 $1
source /usr/local/jobclient/demo/execute_modular.sh $work_log_notice

process_start
if [ $? -ne 0 ]
then
    exit 255
fi

write_log "[INFO] 代码开始执行..."
write_log "[INFO] 主机名:`hostname`"
write_log "[INFO] 日志文件：${LOG_FILE}"
write_log "[INFO] 业务日期：$date, $1"
write_log "[INFO] arguments: $#"
write_log "[INFO] wk dir:"$WK_DIR
write_log "[INFO] result dir:"$RESULT_DIR

#不同的产品线命名不同的配置文件，这样便于区分。
#同时程序也写死了最开始读取的是offline.conf（也可以作为参数传入，但麻烦）
#让麻烦的事情在线下和数据准备阶段解决
function copy_conf() {
    cp ${WK_DIR}/conf/framework/mo_offline.conf ${RESULT_DIR}/offline.conf
    cp ${WK_DIR}/conf/bisheng/mo_bisheng.conf ${RESULT_DIR}/bisheng.conf
    cp ${WK_DIR}/conf/bisheng/mo_feature_list.conf ${RESULT_DIR}/feature_list.conf
}
#如果有hive表，拿到hive表的schema,为了用户方便，改写配置文件,其他配置直接拷贝过来
function rewrite_offline_conf() {
    hive_table_exist=`grep "hive_table=" ${RESULT_DIR}/offline.conf`
    if [ $? -eq 0 ]; then
        a=$(grep "hive_table=" ${RESULT_DIR}/offline.conf)
        hive_table=$(echo $a | awk -F'=' '{print $2}')
        sql="show columns in ${hive_table}"
        columns=`hive -e "$sql"`
        cols=$(echo $columns | sed 's/ /,/g')
        echo "input_schema="$cols >>${RESULT_DIR}/offline.conf
        echo "columns:"$cols" hive_table:"$hive_table
    fi
}
#有些hive表是以rcfile格式存储，改写到能以行为单位解析的格式
function transform_input_data() {
    op_day=$1
    input_dir=$2
    hive_table_exist=`grep "hive_table=" ${RESULT_DIR}/offline.conf`
    if [ $? -eq 0 ]; then
        a=$(grep "hive_table=" ${RESULT_DIR}/offline.conf)
        hive_table=$(echo $a | awk -F'=' '{print $2}')
        sql="insert overwrite directory '${input_dir}' 
            select * from ${hive_table} where dt=${op_day}"
        echo "sql:""$sql"
        hive -e "${sql}"
        echo "hive2hdfs done..."
    fi
}
### 说明：
#       extractor几乎不怎么变，可以放入git中
##  如果需要调试map，则可以将reducer设置为cat
#       -reducer /bin/cat \
##  如果需要将reduce的输出压缩，则可以使用下面的选项
#       -jobconf mapred.output.compression.codec=org.apache.hadoop.io.compress.GzipCodec \
#       -jobconf mapred.output.compress=true \
##  其他值得尝试的选项
#       -jobconf mapred.job.tracker=local \
##  如果hadoop的版本升级，请使用配套的hadoop streaming jar包
#       ${HADOOP_BIN} jar ${WK_DIR}/jar/hadoop-streaming-3.1.0.jar  \
##  如果想对partition进行控制, 使用如下选项(一般不是必须)
#       -D stream.map.output.field.separator=. \
#       -D stream.num.map.output.key.fields=4 \
#       -D map.output.key.field.separator=. \
#       -D mapred.text.key.partitioner.options=-k1,2 \
#       -partitioner org.apache.hadoop.mapred.lib.KeyFieldBasedPartitioner
## 如果想对排序进行控制，使用如下选项(一般也不是必须)
#       -D mapred.output.key.comparator.class=org.apache.hadoop.mapred.lib.KeyFieldBasedComparator \
#       -D stream.map.output.field.separator=. \
#       -D stream.num.map.output.key.fields=4 \
#       -D map.output.key.field.separator=. \
#       -D mapred.text.key.comparator.options=-k2,2nr
## 如果partition和sort都控制，从上面可以看出，有些选项只需要配置一次即可，可参考如下url:
#       https://www.cnblogs.com/joyeecheung/p/3841952.html

HADOOP_BIN=`which hadoop`
function fea_extract() {
    input_dir=$1
    output_dir=$2
    ${HADOOP_BIN} jar /usr/local/hadoop-2.7.3/share/hadoop/tools/lib/hadoop-streaming-2.7.3.jar \
        -libjars ${WK_DIR}/jar/FeaMultiout.jar \
        -input "${input_dir}" \
        -output "${output_dir}" \
        -mapper "${WK_DIR}/bin/extractor map" \
        -combiner "${WK_DIR}/bin/extractor combine" \
        -reducer "${WK_DIR}/bin/extractor reduce" \
        -file ${WK_DIR}/bin/extractor \
        -file ${RESULT_DIR}/bisheng.conf \
        -file ${RESULT_DIR}/feature_list.conf \
        -file ${RESULT_DIR}/offline.conf \
        -outputformat "fea.FeaMultipleOutputFormat" \
        -jobconf mapred.job.name="off_fea_extractor" \
        -jobconf mapred.map.tasks=100 \
        -jobconf mapred.job.map.capacity=100 \
        -jobconf mapred.reduce.tasks=200 \
        -jobconf mapred.job.reduce.capacity=200
}

# 本地调试命令：
#   sh run.sh 20180808 local
# 说明：后面这个日期是没办法，这个脚本支持在调度机，39/41上运行，而调度机一定需要日期参数，我们折中一下。
if [ "$2" == "local" ]; then
    write_log "[INFO] RUN FOR LOCAL"
    copy_conf
    rewrite_offline_conf
    #transform_input_data ${op_day} ${input_dir}
    # hdfs dfs -get ${input_dir}/part-00000 ./test.txt
    cp ${RESULT_DIR}/offline.conf ./ 
    cp ${RESULT_DIR}/bisheng.conf ./ 
    cp ${RESULT_DIR}/feature_list.conf ./ 
    # map->combiner->partitioner->sort->shuffle->sort->reduce
    # 为了保持本地运行和远程运行的一致性，行分隔符不变，使用者确保下面的test.txt文件的分隔符跟配置一致
    #cat ./test.txt | ${WK_DIR}/bin/extractor "map"
    cat ./test.txt | ${WK_DIR}/bin/extractor "map" | ${WK_DIR}/bin/extractor "combine" | sort | ${WK_DIR}/bin/extractor "reduce"
    exit 0
fi

op_day=$1
task=$2
PERSON=model_train
#PERSON=hongqing

write_log "[INFO] op_day:"${op_day}
write_log "[INFO] task:"${task}
write_log "[INFO] whos:"${PERSON}

#input_dir=/dw_ext/ad/sds/sds_ad_algo_sfst_train_data2/dt=${op_day}
op_dir=/user_ext/ads_fst/${PERSON}/xfea/${op_day}/
input_dir=${op_dir}/input
hdfs dfs -mkdir -p ${input_dir}
hdfs dfs -chmod 777 ${input_dir}

#进行必要的配置和数据准备
copy_conf
rewrite_offline_conf
transform_input_data ${op_day} ${input_dir}

write_log "hdfs input:"${input_dir}
#output_dir=hdfs://ns3-backup/user/ads_fst/${PERSON}/xfea/${op_day}
output_dir=${op_dir}/output
hdfs dfs -mkdir -p ${output_dir} 
hdfs dfs -chmod 777 ${output_dir}

if [ $task == "all" ]; then
    write_log "run for all: ig/ir/is"
    {
        igl_output_dir=${output_dir}/${igl}_o
        write_log "hdfs output:"${igl_output_dir}
        hdfs dfs -rmr ${igl_output_dir}
        fea_extract ${input_dir} ${igl_output_dir} 
    }&
    {
        irl_output_dir=${irl_output_dir}/${irl}_o
        hdfs dfs -rmr ${irl_output_dir}
        fea_extract ${input_dir} ${irl_output_dir} 
    }&
    {
        isl_output_dir=${output_dir}/${isl}_o
        hdfs dfs -rmr ${isl_output_dir}
        fea_extract ${input_dir} ${isl_output_dir} 
    }
else
    write_log "run for ${task}"
    new_output_dir=${output_dir}/${task}_o
    write_log "hdfs output:"${new_output_dir}
    hdfs dfs -rmr ${new_output_dir}
    fea_extract ${input_dir} ${new_output_dir} 
fi
wait

write_log "[INFO] end of task:"${task}
process_end
if [ $? -ne 0 ]
then
    exit 255
fi
