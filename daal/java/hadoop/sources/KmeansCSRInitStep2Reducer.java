/* file: KmeansCSRInitStep2Reducer.java */
//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2017 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================

package DAAL;

import java.io.*;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.fs.FileSystem;

import com.intel.daal.data_management.data.HomogenNumericTable;
import com.intel.daal.algorithms.kmeans.*;
import com.intel.daal.algorithms.kmeans.init.*;
import com.intel.daal.data_management.data.*;
import com.intel.daal.services.*;

public class KmeansCSRInitStep2Reducer extends Reducer<IntWritable, WriteableData, IntWritable, WriteableData> {

    private static final long nClusters = 20;

    @Override
    public void reduce(IntWritable key, Iterable<WriteableData> values, Context context)
    throws IOException, InterruptedException {

        DaalContext daalContext = new DaalContext();

        /* Create an algorithm to initialize the K-Means algorithm on the master node */
        InitDistributedStep2Master kmeansMasterInit = new InitDistributedStep2Master(daalContext, Double.class, InitMethod.randomCSR,
                                                                                     nClusters);
        for (WriteableData value : values) {
            InitPartialResult pr = (InitPartialResult)value.getObject(daalContext);
            kmeansMasterInit.input.add( InitDistributedStep2MasterInputId.partialResults, pr );
        }

        /* Initialize the K-Means algorithm on the master node */
        kmeansMasterInit.compute().dispose();

        /* Finalize computations and retrieve the results */
        InitResult initResult = kmeansMasterInit.finalizeCompute();

        /* Write a centroids table for step 3 */
        HomogenNumericTable centroids = (HomogenNumericTable)initResult.get(InitResultId.centroids);

        SequenceFile.Writer writer = SequenceFile.createWriter(
                                         new Configuration(),
                                         SequenceFile.Writer.file(new Path("/Hadoop/KmeansCSR/initResults/centroids")),
                                         SequenceFile.Writer.keyClass(IntWritable.class),
                                         SequenceFile.Writer.valueClass(WriteableData.class));
        writer.append(new IntWritable(0), new WriteableData(centroids));
        writer.close();

        daalContext.dispose();
    }
}
