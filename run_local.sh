#日期无所谓
cur_date=20180907
base_op_dir=local
task=ir
off_conf="offline/conf/offline.conf"
bisheng_conf_dir="bisheng/conf/"
bisheng_root_conf="bisheng_ir_off.conf"
hdfs dfs -copyToLocal /user_ext/ads_fanstop/zhaolei8/fst/xfea/ir_v2/20180826/input/001053_0 ./test1.txt
hdfs dfs -copyToLocal /user_ext/ads_fanstop/zhaolei8/fst/xfea/ir_v2/20180826/input/001039_0 ./test2.txt
sh -x offline/run.sh "$cur_date" ${base_op_dir} $task $off_conf $bisheng_conf_dir $bisheng_root_conf >& log/offline_xfea.local.$task.$cur_date
