/* file: PcaCorCSRStep2Reducer.java */
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
import com.intel.daal.data_management.data.CSRNumericTable;
import com.intel.daal.algorithms.pca.*;
import com.intel.daal.algorithms.PartialResult;
import com.intel.daal.data_management.data.*;
import com.intel.daal.services.*;

public class PcaCorCSRStep2Reducer extends Reducer<IntWritable, WriteableData, IntWritable, WriteableData> {

    @Override
    public void reduce(IntWritable key, Iterable<WriteableData> values, Context context)
                  throws IOException, InterruptedException {

        DaalContext daalContext = new DaalContext();

        /* Create an algorithm to compute PCA decomposition using the correlation method on the master node */
        DistributedStep2Master pcaMaster = new DistributedStep2Master(daalContext, Double.class, Method.correlationDense);

        com.intel.daal.algorithms.covariance.DistributedStep2Master covarianceSparse
            = new com.intel.daal.algorithms.covariance.DistributedStep2Master(daalContext, Double.class,
                                                                              com.intel.daal.algorithms.covariance.Method.fastCSR);
        pcaMaster.parameter.setCovariance(covarianceSparse);

        for (WriteableData value : values) {
            PartialResult pr = (PartialResult)value.getObject(daalContext);
            pcaMaster.input.add( MasterInputId.partialResults, pr );
        }

        /* Compute PCA decomposition on the master node */
        pcaMaster.compute();

        /* Finalize computations and retrieve the results */
        Result res = pcaMaster.finalizeCompute();

        HomogenNumericTable eigenValues  = (HomogenNumericTable)res.get(ResultId.eigenValues );
        HomogenNumericTable eigenVectors = (HomogenNumericTable)res.get(ResultId.eigenVectors);

        context.write(new IntWritable(0), new WriteableData( eigenValues  ) );
        context.write(new IntWritable(1), new WriteableData( eigenVectors ) );

        daalContext.dispose();
    }
}
