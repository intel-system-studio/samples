# file: daal_alexnet.py
# ==============================================================
#
# SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
# http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
#
# Copyright 2017 Intel Corporation
#
# THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
#
# =============================================================

#
# !  Content:
# !    Python example of neural network training and scoring with AlexNet topology
# !*****************************************************************************
from __future__ import division

import sys
from os.path import join as jp
import numpy as np

from daal.algorithms import optimization_solver
from daal.algorithms.neural_networks import layers
from daal.algorithms.neural_networks import training, prediction
from daal.algorithms.neural_networks.layers import (
    convolution2d, relu, lrn, maximum_pooling2d, fullyconnected, dropout, pooling2d
)
from daal.algorithms.neural_networks.initializers import uniform, gaussian
from daal.data_management import HomogenNumericTable, NumericTableIface

from blob_dataset import ImageBlobDatasetReader
from service import ClassificationErrorCounter, getUserDatasetPath, selectDatasetPathOrExit


def configureNet():

    # convolution: 11x11@96 + 4x4s
    convolution1 = convolution2d.Batch()
    convolution1.parameter.kernelSizes = convolution2d.KernelSizes(11, 11)
    convolution1.parameter.strides = convolution2d.Strides(4, 4)
    convolution1.parameter.nKernels = 96
    convolution1.parameter.weightsInitializer = gaussian.Batch(0, 0.01)
    convolution1.parameter.biasesInitializer = uniform.Batch(0, 0)

    # relu
    relu1 = relu.Batch()

    # lrn: alpha=0.0001, beta=0.75, local_size=5
    lrn1 = lrn.Batch()
    lrn1.parameter.kappa = 1
    lrn1.parameter.nAdjust = 5
    lrn1.parameter.alpha = 0.0001 / lrn1.parameter.nAdjust
    lrn1.parameter.beta = 0.75

    # pooling: 3x3 + 2x2s
    maxpooling1 = maximum_pooling2d.Batch(4)
    maxpooling1.parameter.kernelSizes = pooling2d.KernelSizes(3, 3)
    maxpooling1.parameter.paddings = pooling2d.Paddings(0, 0)
    maxpooling1.parameter.strides = pooling2d.Strides(2, 2)

    # convolution: 5x5@256 + 1x1s
    convolution2 = convolution2d.Batch()
    convolution2.parameter.kernelSizes = convolution2d.KernelSizes(5, 5)
    convolution2.parameter.strides = convolution2d.Strides(1, 1)
    convolution2.parameter.paddings = convolution2d.Paddings(2, 2)
    convolution2.parameter.nKernels = 256
    convolution2.parameter.nGroups = 2
    convolution2.parameter.weightsInitializer = gaussian.Batch(0, 0.01)
    convolution2.parameter.biasesInitializer = uniform.Batch(0, 0)

    # relu
    relu2 = relu.Batch()

    # lrn: alpha=0.0001, beta=0.75, local_size=5
    lrn2 = lrn.Batch()
    lrn2.parameter.kappa = 1
    lrn2.parameter.nAdjust = 5
    lrn2.parameter.alpha = 0.0001 / lrn2.parameter.nAdjust
    lrn2.parameter.beta = 0.75

    # pooling: 3x3 + 2x2s
    maxpooling2 = maximum_pooling2d.Batch(4)
    maxpooling2.parameter.kernelSizes = pooling2d.KernelSizes(3, 3)
    maxpooling2.parameter.paddings = pooling2d.Paddings(0, 0)
    maxpooling2.parameter.strides = pooling2d.Strides(2, 2)

    # convolution: 3x3@384 + 2x2s
    convolution3 = convolution2d.Batch()
    convolution3.parameter.kernelSizes = convolution2d.KernelSizes(3, 3)
    convolution3.parameter.paddings = convolution2d.Paddings(1, 1)
    convolution3.parameter.strides = convolution2d.Strides(1, 1)
    convolution3.parameter.nKernels = 384
    convolution3.parameter.weightsInitializer = gaussian.Batch(0, 0.01)
    convolution3.parameter.biasesInitializer = uniform.Batch(0, 0)

    # relu
    relu3 = relu.Batch()

    # convolution: 3x3@384 + 2x2s
    convolution4 = convolution2d.Batch()
    convolution4.parameter.kernelSizes = convolution2d.KernelSizes(3, 3)
    convolution4.parameter.paddings = convolution2d.Paddings(1, 1)
    convolution4.parameter.strides = convolution2d.Strides(1, 1)
    convolution4.parameter.nKernels = 384
    convolution4.parameter.nGroups = 2
    convolution4.parameter.weightsInitializer = gaussian.Batch(0, 0.01)
    convolution4.parameter.biasesInitializer = uniform.Batch(0, 0)

    # relu
    relu4 = relu.Batch()

    # convolution: 3x3@256 + 2x2s
    convolution5 = convolution2d.Batch()
    convolution5.parameter.kernelSizes = convolution2d.KernelSizes(3, 3)
    convolution5.parameter.paddings = convolution2d.Paddings(1, 1)
    convolution5.parameter.strides = convolution2d.Strides(1, 1)
    convolution5.parameter.nKernels = 256
    convolution5.parameter.nGroups = 2
    convolution5.parameter.weightsInitializer = gaussian.Batch(0, 0.01)
    convolution5.parameter.biasesInitializer = uniform.Batch(0, 0)

    # relu
    relu5 = relu.Batch()

    # pooling: 3x3 + 2x2s
    maxpooling5 = maximum_pooling2d.Batch(4)
    maxpooling5.parameter.kernelSizes = pooling2d.KernelSizes(3, 3)
    maxpooling5.parameter.paddings = pooling2d.Paddings(0, 0)
    maxpooling5.parameter.strides = pooling2d.Strides(2, 2)

    # fullyconnected: n = 4096
    fullyconnected6 = fullyconnected.Batch(4096)
    fullyconnected6.parameter.weightsInitializer = gaussian.Batch(0, 0.01)
    fullyconnected6.parameter.biasesInitializer = uniform.Batch(0, 0)

    # relu
    relu6 = relu.Batch()

    # dropout: p = 0.5
    dropout6 = dropout.Batch()
    dropout6.parameter.retainRatio = 0.5

    # fullyconnected: n = 4096
    fullyconnected7 = fullyconnected.Batch(4096)
    fullyconnected7.parameter.weightsInitializer = gaussian.Batch(0, 0.005)
    fullyconnected7.parameter.biasesInitializer = uniform.Batch(0, 0)

    # relu
    relu7 = relu.Batch()

    # dropout: p = 0.5
    dropout7 = dropout.Batch()
    dropout7.parameter.retainRatio = 0.5

    # fullyconnected: n = 1000
    fullyconnected8 = fullyconnected.Batch(1000)
    fullyconnected8.parameter.weightsInitializer = gaussian.Batch(0, 0.01)
    fullyconnected8.parameter.biasesInitializer = uniform.Batch(0, 0)

    # softmax + crossentropy loss
    softmax = layers.loss.softmax_cross.Batch()

    # Create AlexNet Topology
    topology = training.Topology()

    conv1 = topology.add(convolution1   )
    r1    = topology.add(relu1          )
    l1    = topology.add(lrn1           )
    pool1 = topology.add(maxpooling1    )
    conv2 = topology.add(convolution2   )
    r2    = topology.add(relu2          )
    l2    = topology.add(lrn2           )
    pool2 = topology.add(maxpooling2    )
    conv3 = topology.add(convolution3   )
    r3    = topology.add(relu3          )
    conv4 = topology.add(convolution4   )
    r4    = topology.add(relu4          )
    conv5 = topology.add(convolution5   )
    r5    = topology.add(relu5          )
    pool5 = topology.add(maxpooling5    )
    fc6   = topology.add(fullyconnected6)
    r6    = topology.add(relu6          )
    drop6 = topology.add(dropout6       )
    fc7   = topology.add(fullyconnected7)
    r7    = topology.add(relu7          )
    drop7 = topology.add(dropout7       )
    fc8   = topology.add(fullyconnected8)
    sm    = topology.add(softmax        )

    topology.get(conv1).addNext(r1   )
    topology.get(r1   ).addNext(l1   )
    topology.get(l1   ).addNext(pool1)
    topology.get(pool1).addNext(conv2)
    topology.get(conv2).addNext(r2   )
    topology.get(r2   ).addNext(l2   )
    topology.get(l2   ).addNext(pool2)
    topology.get(pool2).addNext(conv3)
    topology.get(conv3).addNext(r3   )
    topology.get(r3   ).addNext(conv4)
    topology.get(conv4).addNext(r4   )
    topology.get(r4   ).addNext(conv5)
    topology.get(conv5).addNext(r5   )
    topology.get(r5   ).addNext(pool5)
    topology.get(pool5).addNext(fc6  )
    topology.get(fc6  ).addNext(r6   )
    topology.get(r6   ).addNext(drop6)
    topology.get(drop6).addNext(fc7  )
    topology.get(fc7  ).addNext(r7   )
    topology.get(r7   ).addNext(drop7)
    topology.get(drop7).addNext(fc8  )
    topology.get(fc8  ).addNext(sm   )

    return topology


def train(reader, batchSize, nTrainImages):
    """Trains AlexNet with given dataset reader"""

    trainingIterations = 1
    learningRate = 0.0005

    print("Training started with batch size = [{}]".format(batchSize))

    # Fetch AlexNet topology (configureNet defined in daal_alexnet.h)
    topology = configureNet()

    # Create the neural network training algorithm and set batch size and optimization solver
    net = training.Batch(getOptimizationSolver(learningRate, batchSize))
    net.parameter.optimizationSolver.getParameter().nIterations = nTrainImages // net.parameter.optimizationSolver.getParameter().batchSize

    # Initialize neural network with given topology
    net.initialize(reader.getBatchDimensions(), topology)

    batchCounter = 0
    for i in range(trainingIterations):
        # Reset reader's iterator the dataset begining
        reader.reset()

        # Advance dataset reader's iterator to the next batch
        while reader.next():

            batchCounter += 1
            # Set the input data batch to the neural network
            net.input.setInput(training.data, reader.getBatch())

            # Set the input ground truth (labels) batch to the neural network
            net.input.setInput(training.groundTruth, reader.getGroundTruthBatch())

            # Compute the neural network forward and backward passes and update
            # weights and biases according to the optimization solver
            net.compute()

            print("{} train batches processed".format(batchCounter))

    # Get prediction model
    trainingResult = net.getResult()
    trainedModel = trainingResult.get(training.model)
    return trainedModel.getPredictionModel_Float32()


def test(predictionModel, reader):
    """Tests AlexNet with given dataset reader"""

    # Create the neural network prediction algorithm
    net = prediction.Batch()

    # Set the prediction model retrieved from the training stage
    net.input.setModelInput(prediction.model, predictionModel)

    # Create auxiliary object to compute error rates (defined in services.h)
    errorRateCounter = ClassificationErrorCounter()

    # Reset reader's iterator the dataset begining
    reader.reset()

    batchCounter = 0

    # Advance dataset reader's iterator to the next batch
    while reader.next():

        batchCounter += 1

        # Set the input data batch to the neural network
        net.input.setTensorInput(prediction.data, reader.getBatch())

        # Compute the neural network forward pass
        net.compute()

        # Get tensor of predicted probailities for each class and update error rate
        predictionResult = net.getResult().getResult(prediction.prediction)
        errorRateCounter.update(predictionResult, reader.getGroundTruthBatch())

        print("{} test batches processed".format(batchCounter))

    return errorRateCounter.getTop5ErrorRate()


def getOptimizationSolver(learningRate, batchSize):
    """Constructs the optimization solver with given learning rate"""

    # Create 1 x 1 NumericTable to store learning rate
    learningRateSequence = HomogenNumericTable(1, 1, NumericTableIface.doAllocate, learningRate, ntype=np.float32)

    # Create SGD optimization solver and set learning rate
    optalg = optimization_solver.sgd.Batch(ntype=np.float32)
    optalg.parameter.learningRateSequence = learningRateSequence
    optalg.parameter.batchSize = batchSize
    return optalg


def main(args):
    defaultDatasetsPath = "./data"
    datasetFileNames = [
        "train_227x227.blob",
        "test_227x227.blob"
    ]

    batchSize = 1
    userDatasetsPath = getUserDatasetPath(args)
    datasetsPath = selectDatasetPathOrExit(defaultDatasetsPath, userDatasetsPath, datasetFileNames, 2)

    # Form path to the training and testing datasets
    trainBlobPath = jp(datasetsPath, datasetFileNames[0])
    testBlobPath = jp(datasetsPath, datasetFileNames[1])

    # Create blob dataset reader for the training dataset (ImageBlobDatasetReader defined in blob_dataset.py)
    with ImageBlobDatasetReader(trainBlobPath, batchSize) as trainDatasetReader:
        nTrainImages = trainDatasetReader.getNumberOfImages()
        predictionModel = train(trainDatasetReader, batchSize, nTrainImages)

    # Create blob dataset reader for the testing dataset
    with ImageBlobDatasetReader(testBlobPath, batchSize) as testDatasetReader:
        top5ErrorRate = test(predictionModel, testDatasetReader)

    print("Top-5 error = {}%".format(top5ErrorRate * 100.0))

if __name__ == '__main__':
    main(sys.argv)
