import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;
import java.io.FileInputStream;
import java.lang.String;
import org.apache.commons.lang.StringUtils;

//import javax.swing.text.html.HTMLDocument.Iterator;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Mapper.Context;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;
import org.apache.hadoop.util.GenericOptionsParser;


public class instance_pro_xfea {
        public static class Map extends Mapper<LongWritable, Text, Text, Text> {
                private Text featu = new Text();
                private Text label = new Text();
                public void map(LongWritable key, Text value, Context context)
                                throws IOException, InterruptedException {
                        String line = value.toString();
                        line = StringUtils.trimToNull(line);
                        //line = line.substring(0,line.length()-2);
                        featu.set(line.toString());
                        label.set("");
                        context.write(null, featu);
                }
        }

        public static class Reduce extends Reducer<Text, Text, Text, Text> {
                private Text featu = new Text();
                private Text label = new Text();
                public void reduce(Text key, Iterable<Text> values, Context context) {
                        try {

                        } catch (Exception e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                        }
                }
        }
        
        

        public static void main(String[] args) throws Exception {
                Configuration conf = new Configuration();
                String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();

                conf.set("mapred.textoutputformat.ignoreseparator", "true");
                conf.set("mapred.textoutputformat.separator", " ");


                //String path = otherArgs[2];
                //System.out.println(new Path(path).toUri());
                //DistributedCache.addCacheFile(new Path(path).toUri(), conf);

                Job job = new Job(conf, "instance_pro_xfea");
                job.setJarByClass(instance_pro_xfea.class);

                job.setOutputKeyClass(Text.class);
                job.setOutputValueClass(Text.class);
                // 设置reduce个数为0i
                job.setNumReduceTasks(0);

                job.setMapperClass(Map.class);
                job.setReducerClass(Reduce.class);

                job.setInputFormatClass(TextInputFormat.class);
                job.setOutputFormatClass(TextOutputFormat.class);

                FileInputFormat.addInputPath(job, new Path(otherArgs[0]));
                FileOutputFormat.setOutputPath(job, new Path(otherArgs[1]));

                job.waitForCompletion(true);
        }
}