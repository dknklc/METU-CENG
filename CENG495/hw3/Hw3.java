import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Partitioner;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import java.util.Iterator;


public class Hw3 {
    
    public static class TotalMapper extends Mapper<LongWritable, Text, Text, LongWritable>{
  
        public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
            if(key.get() > 0){
                String valueString = value.toString();
                String[] song = valueString.split("\t");
                LongWritable duration = new LongWritable(Long.valueOf(song[2]));
                context.write(new Text("Total:"),duration);
            }
        }
    }
    
    public static class TotalReducer extends Reducer<Text,LongWritable,Text ,LongWritable> {
        private LongWritable result = new LongWritable();
        
        public void reduce(Text key , Iterable<LongWritable> values , Context context ) throws IOException, InterruptedException {
            long sum = 0;
            for(LongWritable val : values){
                sum += val.get();
            }
            result.set(sum);
            context.write(key,result);
        }
    }

    public static class AverageMapper extends Mapper<LongWritable, Text, Text, LongWritable>{

        public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
            if(key.get() > 0) {
                String valueString = value.toString();
                String[] song = valueString.split("\t");
                LongWritable duration = new LongWritable(Long.valueOf(song[2]));
                context.write(new Text("Average:"), duration);
            }
        }
    }

    public static class AverageReducer extends Reducer<Text,LongWritable,Text ,DoubleWritable> {
        private DoubleWritable result = new DoubleWritable();

        public void reduce(Text key , Iterable<LongWritable> values , Context context ) throws IOException, InterruptedException {
            long sum = 0;
            double average = 0;
            int index = 0;
            for(LongWritable val : values){
                sum += val.get();
                index++;
            }
            average =(double) sum / (double) index;
            result.set(average);
            context.write(key,result);
        }
    }

    public static class PopuperMapper extends Mapper<LongWritable, Text, Text, IntWritable>{
        private final static IntWritable one = new IntWritable(1);

        public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
            if(key.get() > 0) {
                String valueString = value.toString();
                String[] artist = valueString.split("\t");
                context.write(new Text(artist[0]), one);
            }
        }
    }

    public static class PopulerReducer extends Reducer<Text,IntWritable,Text ,IntWritable> {
        private IntWritable result = new IntWritable();

        public void reduce(Text key , Iterable<IntWritable> values , Context context ) throws IOException, InterruptedException {
            int sum = 0;
            for(IntWritable val : values){
                sum += val.get();
            }
            result.set(sum);
            context.write(key,result);
        }
    }

    public static class ExplicitlyPopularMapper extends Mapper<LongWritable, Text, Text, Text>{

        public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
            if(key.get() > 0) {
                String valueString = value.toString();
                String[] explicit = valueString.split("\t");
                context.write(new Text(explicit[3]), new Text(value));
            }
        }
    }

    public static class ExplicitlyPopularPartitioner extends Partitioner<Text,Text> {
        public int getPartition(Text key , Text value , int numReduceTasks){
            switch(key.toString()){
                case "True":
                    return 0;
                case "False":
                    return 1;
                default:
                    return 0;
            }
        }
    }

    public static class ExplicitlyPopularReducer extends Reducer<Text,Text,Text ,DoubleWritable> {
        private DoubleWritable result = new DoubleWritable();

        public void reduce(Text key , Iterable<Text> values , Context context ) throws IOException, InterruptedException {
            long sum = 0;
            double average = 0;
            int index = 0;
            for(Text val : values){
                String[] str = val.toString().split("\t");
                sum = sum + Long.parseLong(str[5]);
                index++;
            }
            average =(double) sum /(double) index;
            result.set(average);
            context.write(key,result);
        }
    }




    public static class DancebyyearMapper extends Mapper<LongWritable, Text, Text, Text>{
        //private final static IntWritable year = new IntWritable();

        public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
            if(key.get() > 0) {
                String valueString = value.toString();
                String[] explicit = valueString.split("\t");
                int find_year = Integer.parseInt(explicit[4]);
                if(find_year <= 2002){
                    context.write(new Text("<= 2002"),new Text(value));
                }
                else if(find_year > 2002 && find_year <= 2012){
                    context.write(new Text("2002 üstü ve 2012 altı"),new Text(value));
                }
                else{
                    context.write(new Text("2012 üstü"),new Text(value));
                }
                //year.set(find_year);

            }
        }
    }

    public static class DancebyyearPartitioner extends Partitioner<Text,Text> {
        public int getPartition(Text key , Text value , int numReduceTasks){
            if(key.toString().equals("<= 2002")){
                return 0;
            }
            else if(key.toString().equals("2002 üstü ve 2012 altı")){
                return 1;
            }
            else{
                return 2;
            }
        }
    }

    public static class DancebyyearReducer extends Reducer<Text,Text,Text ,DoubleWritable> {
        private DoubleWritable result = new DoubleWritable();

        public void reduce(Text key , Iterable<Text> values , Context context ) throws IOException, InterruptedException {
            double sum = 0;
            double average = 0;
            int index = 0;
            for(Text val : values){
                String[] str = val.toString().split("\t");
                sum = sum + Double.parseDouble(str[6]);
                index++;
            }
            average = sum / index;
            result.set(average);
            context.write(key,result);
        }
    }


    
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        Job job = Job.getInstance(conf, "Hw3");
        job.setJar("Hw3.jar");
        String type = args[0];
        switch(type) {
            case "total":
                job.setMapperClass(TotalMapper.class);
                job.setCombinerClass(TotalReducer.class);
                job.setReducerClass(TotalReducer.class);
                job.setOutputKeyClass(Text.class);
                job.setOutputValueClass(LongWritable.class);
                break;
            case "average":
                job.setMapperClass(AverageMapper.class);
                //job.setCombinerClass(AverageReducer.class);
                job.setReducerClass(AverageReducer.class);
                job.setOutputKeyClass(Text.class);
                job.setOutputValueClass(LongWritable.class);
                break;
            case "popular":
                job.setMapperClass(PopuperMapper.class);
                job.setCombinerClass(PopulerReducer.class);
                job.setReducerClass(PopulerReducer.class);
                job.setOutputKeyClass(Text.class);
                job.setOutputValueClass(IntWritable.class);
                break;
            case "explicitlypopular":
                job.setMapperClass(ExplicitlyPopularMapper.class);
                //job.setCombinerClass(ExplicitlyPopularReducer.class);
                job.setPartitionerClass(ExplicitlyPopularPartitioner.class);
                job.setReducerClass(ExplicitlyPopularReducer.class);
                job.setNumReduceTasks(2);
                job.setOutputKeyClass(Text.class);
                job.setOutputValueClass(Text.class);
	break;
            case "dancebyyear":
                job.setMapperClass(DancebyyearMapper.class);
                //job.setCombinerClass(ExplicitlyPopularReducer.class);
                job.setPartitionerClass(DancebyyearPartitioner.class);
                job.setReducerClass(DancebyyearReducer.class);
                job.setNumReduceTasks(3);
                job.setOutputKeyClass(Text.class);
                job.setOutputValueClass(Text.class);
	break;
        }


        FileInputFormat.addInputPath(job, new Path(args[1]));
        FileOutputFormat.setOutputPath(job, new Path(args[2]));
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}
