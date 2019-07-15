要验证转换后的额特征是否正确：
1. 聚合后的view clk是否正确
2. 是否只有一行
本地运行：
cat ./test.txt | python ./mapper.py 4=1655074117,6=4272975378837470 | sort | python reducer.py
cat ./test_fea.txt | python ./fea_mapper.py 4194937891619345_5669425046 | sort | python reducer.py
远程运行：
hive -e "select count(*) from sds_ad_algo_sfst_train_data2 where dt=20180818 and feedid=4194937891619345 and custid=5669425046;"
sh run.sh
hdfs dfs -getmerge /user_ext/ads_fst/hongqing/xfea/tmp ./tmp/aaa
