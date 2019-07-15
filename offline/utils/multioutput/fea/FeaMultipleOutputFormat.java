package fea;

import java.util.Arrays;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.lib.MultipleTextOutputFormat;

public class FeaMultipleOutputFormat extends MultipleTextOutputFormat<Text, Text> {

    /**
     * 样本或者特征的'\t'分割的第一个字段被当作key，"instance"和"feature"被放到value的末尾了 
     * 生成"instance/part-00000"或者"feature/part-00000"
     */
    @Override
    protected String generateFileNameForKeyValue(Text key, Text value, String name) 
    {
        String strValue = value.toString();
        int valueLength = strValue.length();
        String outputName = name;
        String[] parts=strValue.split("\t");
        /*
        int index = strValue.lastIndexOf("#");
        if(-1 != index && (index+2) == valueLength)
        {
            String newValue = strValue.substring(0, index);
            value.set(newValue);
            char flag = strValue.charAt(index+1);
            outputName = name+"-"+flag;
        }
        */
        outputName=parts[parts.length - 1] + "/" + name;
        return outputName;
    }
    /*
    @Override
    protected Text generateActualKey(Text key, Text value) {
        return null;
    }
    */

    @Override
    protected Text generateActualValue(Text key, Text value) {
        String strValue = value.toString();
        String[] parts=strValue.split("\t");
        String r_parts = String.join("\t", Arrays.copyOf(parts, parts.length-1));
        return new Text(r_parts);
    }
}

