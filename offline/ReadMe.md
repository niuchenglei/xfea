# 使用方法
1. sh build.sh 注意39.41上面java版本太低编译java会报错，建议在39或者86上编译
2. ```请在上一级目录执行```sh offline/run.sh 20180810 isl（建议不同产品线命名不同的run.sh，比如多目标用run_mo.sh）

# 注意事项
1. 如果使用hive表，而hive表的原始数据的schema跟bisheng的schema不一致，则需要提供hive表名，以方便程序自动获取hive表的schema，并进行列匹配
2. 如果直接使用hdfs的文件，则不需要设置hive表，而需要确保bisheng的schema里头字段的顺序跟hdfs文件里头的列是对应的
3. 为减少麻烦，不必要地记忆这些规则，建议schema跟你要取的字段等信息在顺序、数量和命名上都统一起来
4. offline.conf主要配置跟离线框架有关的内容；其他bisheng的配置还放在bisheng的目录下，这样也方便保持线上线下的一致。
