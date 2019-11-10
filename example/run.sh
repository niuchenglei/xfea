#!/usr/bin/bash
#################################################
# # # Author: xfea-team                     #
# # # Date: 2019-07-23                      #
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
export WK_DIR=`pwd`

op_day=$1
xfea_conf_name=$2
xfea_conf=${WK_DIR}/conf/$bisheng_root_conf_name
input=$3
output=$4


function write_log() {
    echo $@
}

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

function get_bisheng_conf_files() {
    fea_list_conf_name=`grep "feature_list=" ${xfea_conf} | grep -v '#'`
    if [[ $? -eq 0 && ! -z $fea_list_conf_name ]]; then
        #a=$(grep "feature_list=" ${xfea_conf} | grep -v '#')
        feature_list_conf_name=$(echo ${fea_list_conf_name} | awk -F'=' '{print $2}')
        feature_list_conf=${WK_DIR}/conf/${feature_list_conf_name}
        echo "get file name:"$feature_list_conf
    else
        echo "feature list conf not defined"
    fi
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
        -libjars ${WK_DIR}/jar/FeaMultiout.jar \
        -input "${input_dir}" \
        -output "${output_dir}" \
        -mapper "${WK_DIR}/bin/extractor map" \
        -combiner "${WK_DIR}/bin/extractor combine" \
        -reducer "${WK_DIR}/bin/extractor reduce" \
        -file ${WK_DIR}/bin/extractor \
        -file ${xfea_conf} \
        -file ${feature_list_conf} \
        -outputformat "fea.FeaMultipleOutputFormat" \
        -jobconf mapred.job.name="${PERSON}_xfea_extractor" \
        -jobconf mapred.map.tasks=100 \
        -jobconf mapred.job.map.capacity=100 \
        -jobconf mapred.reduce.tasks=200 \
        -jobconf mapred.job.reduce.capacity=200
    
    ${HADOOP_BIN} dfs -rm -r ${output_dir}/instance_back 
    ${HADOOP_BIN} jar ${WK_DIR}/jar/instance_pro_xfea.jar instance_pro_xfea ${output_dir}/instance ${output_dir}/instance_back
    ${HADOOP_BIN} dfs -rm -r ${output_dir}/instance
    ${HADOOP_BIN} dfs -mv ${output_dir}/instance_back ${output_dir}/instance
}

write_log "[INFO] op_day:"${op_day}
write_log "[INFO] whoami:"${PERSON}

#获取bisheng的所有配置文件
get_bisheng_conf_files

write_log "hdfs input:"${input}
write_log "hdfs output:"${output}
hdfs dfs -mkdir -p ${output} 
hdfs dfs -chmod 777 ${output}

fea_extract ${input} ${output} 

write_log "[INFO] end of task"

