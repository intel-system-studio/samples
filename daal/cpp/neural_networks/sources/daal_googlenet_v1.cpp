/* file: daal_googlenet_v1.cpp */
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
!  Content:
!    C++ example of neural network training and scoring with GoogleNetV1 topology
!******************************************************************************/

#include "daal_commons.h"
#include "daal_googlenet_v1.h"

const std::string defaultDatasetsPath = "./data";
const std::string datasetFileNames[] =
{
    "train_224x224.blob",
    "test_224x224.blob"
};

int main(int argc, char *argv[])
{
    std::string userDatasetsPath = getUserDatasetPath(argc, argv);
    std::string datasetsPath = selectDatasetPathOrExit(
        defaultDatasetsPath, userDatasetsPath, datasetFileNames, 2);

    /* Form path to the training and testing datasets */
    std::string trainBlobPath = datasetsPath + "/" + datasetFileNames[0];
    std::string testBlobPath  = datasetsPath + "/" + datasetFileNames[1];

    /* Create blob dataset reader for the training dataset (ImageBlobDatasetReader defined in blob_dataset.h)  */
    ImageBlobDatasetReader<float> trainDatasetReader(trainBlobPath, batchSize);
    training::TopologyPtr topology = configureNet(); /* defined in daal_googlenet_v1.h */

    /* Train model (trainClassifier is defined in daal_common.h) */
    prediction::ModelPtr predictionModel = trainClassifier(topology, &trainDatasetReader);

    /* Create blob dataset reader for the testing dataset */
    ImageBlobDatasetReader<float> testDatasetReader(testBlobPath, batchSize);

    /* Test model (testClassifier is defined in daal_common.h) */
    float top5ErrorRate = testClassifier(predictionModel, &testDatasetReader);

    std::cout << "Top-5 error = " << top5ErrorRate * 100.0 << "%" << std::endl;

    return 0;
}
