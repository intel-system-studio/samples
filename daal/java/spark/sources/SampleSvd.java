/* file: SampleSvd.java */
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
 //     Java sample of computing singular value decomposition (SVD)
 ////////////////////////////////////////////////////////////////////////////////
 */

package DAAL;

import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;

import org.apache.spark.api.java.*;
import org.apache.spark.api.java.function.*;
import org.apache.spark.SparkConf;

import scala.Tuple2;

import com.intel.daal.data_management.data.*;
import com.intel.daal.data_management.data_source.*;
import com.intel.daal.services.*;

public class SampleSvd {
    public static void main(String[] args) {
        DaalContext context = new DaalContext();

        /* Create JavaSparkContext that loads defaults from the system properties and the classpath and sets the name */
        JavaSparkContext sc = new JavaSparkContext(new SparkConf().setAppName("Spark SVD"));

        /* Read from the distributed HDFS data set at a specified path */
        StringDataSource templateDataSource = new StringDataSource( context, "" );
        DistributedHDFSDataSet dd = new DistributedHDFSDataSet( "/Spark/Svd/data/", templateDataSource );
        JavaPairRDD<Integer, HomogenNumericTable> dataRDD = dd.getAsPairRDD(sc);

        /* Compute SVD decomposition for dataRDD */
        SparkSvd.SvdResult result = SparkSvd.runSVD(context, dataRDD, sc);

        /* Print the results */
        List<Tuple2<Integer, HomogenNumericTable>> ntRPList = result.U.collect();
        for (Tuple2<Integer, HomogenNumericTable> value : ntRPList) {
            value._2.unpack( context );
            printNumericTable("U (2 first vectors from node #" + value._1 + "):", value._2, 2);
        }
        printNumericTable("Sigma:", result.Sigma);
        printNumericTable("V:",     result.V);

        context.dispose();
        sc.stop();
    }

    private static void printNumericTable(String header, HomogenNumericTable nt, long nRows) {
        long nCols = nt.getNumberOfColumns();
        double[] result = nt.getDoubleArray();

        int resultIndex = 0;
        StringBuilder builder = new StringBuilder();
        builder.append(header);
        builder.append("\n");
        for (long i = 0; i < nRows; i++) {
            for (long j = 0; j < nCols; j++) {
                String tmp = String.format("%-6.3f   ", result[resultIndex++]);
                builder.append(tmp);
            }
            builder.append("\n");
        }
        System.out.println(builder.toString());
    }

    private static void printNumericTable(String header, HomogenNumericTable nt) {
        printNumericTable(header, nt, nt.getNumberOfRows());
    }
}
