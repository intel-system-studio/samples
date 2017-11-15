/* file: CovarianceCSRStep2Reducer.java */
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

package DAAL;

import java.io.*;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.fs.FileSystem;

import com.intel.daal.data_management.data.HomogenNumericTable;
import com.intel.daal.algorithms.covariance.*;
import com.intel.daal.data_management.data.*;
import com.intel.daal.services.*;

public class CovarianceCSRStep2Reducer extends Reducer<IntWritable, WriteableData, IntWritable, WriteableData> {

    @Override
    public void reduce(IntWritable key, Iterable<WriteableData> values, Context context)
    throws IOException, InterruptedException {

        DaalContext daalContext = new DaalContext();

        /* Create an algorithm to compute a sparse variance-covariance matrix on the master node */
        DistributedStep2Master covarianceSparseMaster = new DistributedStep2Master(daalContext, Double.class, Method.fastCSR);

        for (WriteableData value : values) {
            PartialResult pr = (PartialResult)value.getObject(daalContext);
            covarianceSparseMaster.input.add( DistributedStep2MasterInputId.partialResults, pr );
        }

        /* Compute a sparse variance-covariance matrix on the master node */
        covarianceSparseMaster.compute();

        /* Finalize computations and retrieve the results */
        Result result = covarianceSparseMaster.finalizeCompute();

        HomogenNumericTable covariance = (HomogenNumericTable)result.get(ResultId.covariance);
        HomogenNumericTable mean       = (HomogenNumericTable)result.get(ResultId.mean);

        context.write(new IntWritable(0), new WriteableData( covariance  ) );
        context.write(new IntWritable(1), new WriteableData( mean ) );

        daalContext.dispose();
    }
}
