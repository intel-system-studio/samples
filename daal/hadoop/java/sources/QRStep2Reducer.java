/* file: QRStep2Reducer.java */
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

import java.io.OutputStreamWriter;
import java.io.BufferedWriter;
import java.io.PrintWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.Set;

import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.fs.FileSystem;

import com.intel.daal.data_management.data.HomogenNumericTable;
import com.intel.daal.algorithms.qr.*;
import com.intel.daal.data_management.data.*;
import com.intel.daal.services.*;

public class QRStep2Reducer extends Reducer<IntWritable, WriteableData, IntWritable, WriteableData> {

    private static final int nFeatures = 18;
    private static final int nVectors = 16000;
    private static final int nVectorsInBlock = 4000;
    private int nBlocks;
    private static Configuration config;
    private int index = 0;
    private int totalTasks = 0;

    @Override
    public void setup(Context context) {
        config = context.getConfiguration();
        totalTasks = config.getInt("mapred.map.tasks", 0);
        index = context.getTaskAttemptID().getTaskID().getId();
    }

    @Override
    public void reduce(IntWritable key, Iterable<WriteableData> values, Context context)
    throws IOException, InterruptedException {

        List<Integer> knownIds = new ArrayList<Integer>();

        DaalContext daalContext = new DaalContext();

        /* Create an algorithm to compute QR decomposition on the master node */
        DistributedStep2Master qrStep2Master = new DistributedStep2Master( daalContext, Double.class, Method.defaultDense );

        for (WriteableData value : values) {
            DataCollection dc = (DataCollection)value.getObject(daalContext);
            qrStep2Master.input.add( DistributedStep2MasterInputId.inputOfStep2FromStep1, value.getId(), dc );
            knownIds.add( value.getId() );
        }

        /* Compute QR decomposition in step 2 */
        DistributedStep2MasterPartialResult pres = qrStep2Master.compute();

        KeyValueDataCollection inputForStep3FromStep2 = pres.get( DistributedPartialResultCollectionId.outputOfStep2ForStep3 );

        for (Integer value : knownIds) {
            DataCollection dc = (DataCollection)inputForStep3FromStep2.get( value.intValue() );
            context.write(new IntWritable( value.intValue() ),
                          new WriteableData( value.intValue(), dc ));
        }

        Result res = qrStep2Master.finalizeCompute();
        NumericTable r = res.get( ResultId.matrixR );

        SequenceFile.Writer writer = SequenceFile.createWriter(
                                         new Configuration(),
                                         SequenceFile.Writer.file(new Path("/Hadoop/QR/Output/R")),
                                         SequenceFile.Writer.keyClass(IntWritable.class),
                                         SequenceFile.Writer.valueClass(WriteableData.class));
        writer.append(new IntWritable(0), new WriteableData( r ));
        writer.close();

        daalContext.dispose();
    }
}
