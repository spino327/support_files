public class MxvMap extends Mapper<LongWritable, Text, IntWritable, DoubleWritable>{
    
    protected void setup(
            Mapper<LongWritable, Text, IntWritable, DoubleWritable>.Context context)
            throws IOException, InterruptedException {
       // read vector x 
    }
    
    protected void map(LongWritable key, Text value,
            Mapper<LongWritable, Text, IntWritable, DoubleWritable>.Context context)
            throws IOException, InterruptedException {
        
        String[] split_line = value.toString().split(",");
            
        int i = Integer.parseInt(split_line[0]);
        int j = Integer.parseInt(split_line[1]);
        double aij = Double.parseDouble(split_line[2]);
            
        // y_i = sum aij*xj
        row.set(i);
        aij_x_vj.set(aij * x_i[j]);
            
        context.write(row, aij_x_vj);
    }
}

public class MxvRed extends Reducer<IntWritable, DoubleWritable, IntWritable, DoubleWritable>{
    protected void reduce(
            IntWritable row,
            Iterable<DoubleWritable> values,
            Reducer<IntWritable, DoubleWritable, IntWritable, DoubleWritable>.Context context)
            throws IOException, InterruptedException {
        
        double result = 0;
        for (DoubleWritable val : values) {
            result += val.get();
        }
        yi.set(result);
        context.write(row, yi);
    }
}

public class Mxv {
    
    public static void main(String[] args) {
        Job job = new Job(conf);
        
        // mapper
        job.setMapperClass(MxvMap.class);
        job.setMapOutputKeyClass(IntWritable.class);
        job.setMapOutputValueClass(DoubleWritable.class);
        
        // reducer
        job.setReducerClass(MxvRed.class);
        job.setOutputKeyClass(IntWritable.class);
        job.setOutputValueClass(DoubleWritable.class);
        
        FileInputFormat.addInputPath(job, new Path(input_matrix));
        FileOutputFormat.setOutputPath(job, new Path(output));
        
        return job.waitForCompletion(true) ? 0 : 1;
    }
}
