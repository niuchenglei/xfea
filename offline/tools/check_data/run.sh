#input_dir=/user_ext/ads_fst/model_train/xfea/20180818/output/irl_o/feature
input_dir=/user_ext/ads_fst/model_train/xfea/20180818/input
output_dir=/user_ext/ads_fst/hongqing/xfea/tmp
hdfs dfs -rmr ${output_dir}
HADOOP_BIN=`which hadoop`
${HADOOP_BIN} jar /usr/local/hadoop-2.7.3/share/hadoop/tools/lib/hadoop-streaming-2.7.3.jar \
        -input "${input_dir}" \
        -output "${output_dir}" \
        -file ./mapper.py \
        -mapper "python mapper.py 4=5669425046,6=4194937891619345" \
        -file ./reducer.py \
        -reducer "python reducer.py" \
        -jobconf mapred.job.name="check_data" \
        -jobconf mapred.map.tasks=10 \
        -jobconf mapred.job.map.capacity=10 \
        -jobconf mapred.reduce.tasks=20 \
        -jobconf mapred.job.reduce.capacity=20
