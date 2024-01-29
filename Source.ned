//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// A module that generates jobs. One can specify the number of jobs to be generated,
// the starting and ending time, and interval between generating jobs.
// Job generation stops when the number of jobs or the end time has been reached,
// whichever occurs first. The name, type and priority of jobs can be set as well.
//
simple Source
{
    parameters:
        @signal[created](type="long");
        @statistic[created](title="the number of jobs created";record=last;interpolationmode=none);
        @display("i=block/source");
        volatile double interArrivalTime @unit(s) = default(exponential(100ms)); // time between generated jobs
        string taskName = default("task");         // the base name of the generated job (will be the module name if left empty)
    gates:
        output out;
}

