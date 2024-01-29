//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Destroys (or optionally keep) the packets and collects statistics
//
simple Sink
{
    parameters:
        @display("i=block/sink");
        @signal[lifeTime](type="simtime_t");
        @signal[totalWaitingTime](type="simtime_t");
        @signal[totalProcessingTime](type="simtime_t");
        @signal[totalPropagationTime](type="simtime_t");
        @signal[deadline](type="simtime_t");
        @signal[taskSize](type="double");
        @signal[cpuCycles](type="double");
        @signal[processedCycles](type="double");
        @signal[arrivingServer](type="string");
        @signal[runningServer](type="string");
        @statistic[lifeTime](title="lifetime of arrived jobs";unit=s;record=vector,mean,max;interpolationmode=none);
        @statistic[totalWaitingTime](title="the total time spent in waiting queue";unit=s;record=vector?,mean,max;interpolationmode=none);
        @statistic[totalProcessingTime](title="the total time spent in processing the task";unit=s;record=vector?,mean,max;interpolationmode=none);
        @statistic[totalPropagationTime](title="the total time spent on the link";unit=s;record=vector?,mean,max;interpolationmode=none);
        @statistic[deadline](title="the task maximum delay allowed";record=vector?,mean,max;interpolationmode=none);
        @statistic[taskSize](title="the size of the task";record=vector?,mean,max;interpolationmode=none);
        @statistic[cpuCycles](title="the required cpu cycles to process the task";record=vector?,mean,max;interpolationmode=none);
        @statistic[processedCycles](title="the processed cycles of the task";record=vector?,mean,max;interpolationmode=none);
        @statistic[arrivingServer](title="starting server of task";record=vector);
        @statistic[runningServer](title="running server of task";record=vector);    
    gates:
        input in[];
}

