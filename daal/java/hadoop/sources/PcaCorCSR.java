/* file: PcaCorCSR.java */
//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright (C) Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================

/*
 //  Content:
 //     Java sample of principal component analysis (PCA) using the correlation
 //     method with data in compressed sparse rows (CSR) format
 //     in the distributed processing mode
 ////////////////////////////////////////////////////////////////////////////////
 */

package DAAL;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.filecache.DistributedCache;
import java.net.URI;

import com.intel.daal.data_management.data.*;
import com.intel.daal.data_management.data_source.*;
import com.intel.daal.services.*;

/* Implement Tool to be able to pass -libjars on start */
public class PcaCorCSR extends Configured implements Tool {

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new PcaCorCSR(), args);
        System.exit(res);
    }

    @Override
    public int run(String[] args) throws Exception {
        Configuration conf = this.getConf();

        /* Put shared libraries into the distributed cache */
        DistributedCache.createSymlink(conf);
        DistributedCache.addCacheFile(new URI("/Hadoop/Libraries/" + System.getenv("LIBJAVAAPI")), conf);
        DistributedCache.addCacheFile(new URI("/Hadoop/Libraries/" + System.getenv("LIBTBB")), conf);
        DistributedCache.addCacheFile(new URI("/Hadoop/Libraries/" + System.getenv("LIBTBBMALLOC")), conf);

        Job job = new Job(conf, "PCA Job");

        FileInputFormat.setInputPaths(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        job.setMapperClass(PcaCorCSRStep1Mapper.class);
        job.setReducerClass(PcaCorCSRStep2Reducer.class);

        job.setInputFormatClass(TextInputFormat.class);
        job.setOutputFormatClass(SequenceFileOutputFormat.class);
        job.setOutputKeyClass(IntWritable.class);
        job.setOutputValueClass(WriteableData.class);

        job.setJarByClass(PcaCorCSR.class);

        return job.waitForCompletion(true) ? 0 : 1;
    }
}
