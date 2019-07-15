#!/usr/bin/bash
#################################################
# # # Author: xfea-team                     #
# # # Date: 2018-08-16                      #
# # # Describe: # 模板                      #
# # #############################################
# # ############# Config init ...... ############
# # # 参数1: 操作哪天的数据                 #
# # #     2: 数据处理操作目录               #
# # #     3: task，对于多目标来说，比如irl  #
# # #     4：离线配置                       #
# # #     5: bisheng配置存放目录            #
# # #     6: bisheng根配置                  #
# # #############################################
op_day=$1
base_op_dir=$2
task=$3
config=$4
bisheng_conf_dir=$5
bisheng_root_conf=$5"/"$6
bisheng_root_conf_name=$6

export WK_DIR=`pwd`
source /usr/local/jobclient/config/.hive_config.sh ${op_day}
source /usr/local/jobclient/lib/source $0 ${op_day}
source /usr/local/jobclient/demo/execute_modular.sh $work_log_notice

process_start
if [ $? -ne 0 ]
then
    echo "fail to start process"
    exit 255
fi

OFFLINE_DIR=${WK_DIR}/offline/
BISHENG_DIR=${WK_DIR}/bisheng/

# 39、41环境跟调度环境保持一致，都可以执行
write_log "[INFO] 代码开始执行..."
write_log "[INFO] 主机名:`hostname`"
write_log "[INFO] 日志文件：${LOG_FILE}"
write_log "[INFO] 业务日期：$date, $1"
write_log "[INFO] arguments: $#"
write_log "[INFO] wk dir:"$WK_DIR
write_log "[INFO] result dir:"$RESULT_DIR

feature_list_conf=""
feature_list_conf_name=""
ctr_map_txt=""
ctr_map_txt_name=""

function get_bisheng_conf_files() {
    fea_list_conf_name=`grep "feature_list=" ${bisheng_root_conf} | grep -v '#'`
    if [[ $? -eq 0 && ! -z $fea_list_conf_name ]]; then
        #a=$(grep "feature_list=" ${bisheng_root_conf} | grep -v '#')
        feature_list_conf_name=$(echo ${fea_list_conf_name} | awk -F'=' '{print $2}')
        feature_list_conf=${bisheng_conf_dir}/${feature_list_conf_name}
        echo "get file name:"$feature_list_conf
    else
        echo "feature list conf not defined"
    fi

    ctr_map_conf_name=`grep "ctr_bin_file=" ${bisheng_root_conf} | grep -v '#'`
    if [[ $? -eq 0  && ! -z $ctr_map_conf_name ]]; then
        #a=$(grep "ctr_bin_file=" ${bisheng_root_conf} | grep -v '#')
        ctr_map_txt_name=$(echo ${ctr_map_conf_name} | awk -F'=' '{print $2}')
        ctr_map_txt=${bisheng_conf_dir}/${ctr_map_txt_name}
        echo "get file name:"$ctr_map_txt
    else
        echo "ctr bin file not defined"
    fi
}
#不同的产品线命名不同的离线和bisheng配置文件，这样便于区分。
#这里拷贝到可写目录并把离线配置文件重命名，其他文件名保持不变。
#这么做是为了方便离线处理程序读配置，程序里头写死了从本地目录读取的是offline.conf
#最灵活的方式是不改名字，作为参数传入，但会引起hadoop streaming的参数变长 
#而且文件一旦上传到hadoop和调度系统上，一般都是把文件存放到本地。
function copy_conf() {
    cp ${config} ${RESULT_DIR}/offline.conf
    cp ${bisheng_root_conf} ${RESULT_DIR}/
    cp ${feature_list_conf} ${RESULT_DIR}/
    cp ${ctr_map_txt} ${RESULT_DIR}/
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
### 说明：
#       1. extractor几乎不怎么变，可以放入git中
#       2. streaming流程可以这么理解: map_pre->streaming_map->map_after===(partition&sort)==>reduce_pre->streaming_reduce->reduce_after
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
## 控制项分类：
#       1. stream打头的，都是控制stream本身的，即pipemap->your streaming map->pipemap
#       2. map打头的是整个map过程的控制项, 跟mapred的配置项协同处理。reduce同理
#       3. mapred打头的是mapreduce框架本身的，一般涉及分区和排序
#       4. 如果不需要对partition和comparator做控制，则map自身的配置也没必要。那样stream的配置就起主要作用，比如不配置comparator，则stream的输出就用于排序
#       5. 如有疑问，查看streaming的源码

HADOOP_BIN=`which hadoop`
function fea_extract() {
    input_dir=$1
    output_dir=$2
    ${HADOOP_BIN} jar /usr/local/hadoop-2.7.3/share/hadoop/tools/lib/hadoop-streaming-2.7.3.jar \
        -libjars ${OFFLINE_DIR}/jar/FeaMultiout.jar \
        -input "${input_dir}" \
        -output "${output_dir}" \
        -mapper "${OFFLINE_DIR}/bin/extractor map" \
        -combiner "${OFFLINE_DIR}/bin/extractor combine" \
        -reducer "${OFFLINE_DIR}/bin/extractor reduce" \
        -file ${OFFLINE_DIR}/bin/extractor \
        -file ${RESULT_DIR}/offline.conf \
        -file ${RESULT_DIR}/${bisheng_root_conf_name} \
        -file ${RESULT_DIR}/${feature_list_conf_name} \
        -file ${RESULT_DIR}/${ctr_map_txt_name} \
        -outputformat "fea.FeaMultipleOutputFormat" \
        -jobconf mapred.job.name="${PERSON}_off_fea_extractor" \
        -jobconf mapred.map.tasks=100 \
        -jobconf mapred.job.map.capacity=100 \
        -jobconf mapred.reduce.tasks=200 \
        -jobconf mapred.job.reduce.capacity=200
    
    ${HADOOP_BIN} dfs -rm -r ${output_dir}/instance_back 
    ${HADOOP_BIN} jar $OFFLINE_DIR/jar/instance_pro_xfea.jar instance_pro_xfea ${output_dir}/instance ${output_dir}/instance_back
    ${HADOOP_BIN} dfs -rm -r ${output_dir}/instance
    ${HADOOP_BIN} dfs -mv ${output_dir}/instance_back ${output_dir}/instance
}

# 本地调试命令：
#   sh run.sh 20180808 local ir offline.conf ./bisheng/conf ./bisheng.conf
# 说明：后面这个日期是没办法，这个脚本支持在调度机，39/41上运行，而调度机一定需要日期参数，我们折中一下。
if [ "$2" == "local" ]; then
    write_log "[INFO] RUN FOR LOCAL"
    get_bisheng_conf_files
    copy_conf
    cp ${RESULT_DIR}/${bisheng_root_conf_name} ./ 
    cp ${RESULT_DIR}/${feature_list_conf_name} ./ 
    cp ${RESULT_DIR}/${ctr_map_txt_name} ./ 
    # map->combiner->partitioner->sort->shuffle->sort->reduce
    # 为了保持本地运行和远程运行的一致性，行分隔符不变，使用者确保下面的test.txt文件的分隔符跟配置一致
    #cat ./test.txt | ${WK_DIR}/bin/extractor "map"
    #cat ./test.txt | ${WK_DIR}/bin/extractor "map" | ${WK_DIR}/bin/extractor "combine" | sort | ${WK_DIR}/bin/extractor "reduce"
    cat ./test1.txt | ${WK_DIR}/bin/extractor "map" | sort | ${WK_DIR}/bin/extractor "combine" >./result1.txt
    cat ./test2.txt | ${WK_DIR}/bin/extractor "map" | sort | ${WK_DIR}/bin/extractor "combine" >./result2.txt
    cat ./result1.txt ./result2.txt | sort | ${WK_DIR}/bin/extractor "reduce" >./result.txt
    exit 0
fi

write_log "[INFO] op_day:"${op_day}
write_log "[INFO] task:"${task}
write_log "[INFO] whoami:"${PERSON}

op_dir=${base_op_dir}/${op_day}/
input_dir=${op_dir}/input
hdfs dfs -mkdir -p ${input_dir}
hdfs dfs -chmod 777 ${input_dir}

#获取bisheng的所有配置文件
get_bisheng_conf_files
#进行必要的配置和数据准备
copy_conf

write_log "hdfs input:"${input_dir}
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
