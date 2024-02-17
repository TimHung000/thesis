simple Processor
{
    parameters:
        @display("i=block/server");
        @signal[memoryLoading](type="double");
        @signal[CPULoading](type="double");        
        @signal[totalTaskFinished](type="double");
        @signal[totalTaskCompleted](type="double");
        @statistic[memoryLoading](title="double";source="memoryLoading";record=vector);
        @statistic[CPULoading](title="double";source="CPULoading";record=vector);
        @statistic[totalTaskFinished](title="totalTaskFinished";source="totalTaskFinished";record=vector);
        @statistic[totalTaskCompleted](title="totalTaskCompleted";source="totalTaskCompleted";record=vector);
        double statusUpadteInterval;  // ms
        double maxFrequency;          // GHz
        double minFrequency;          // GHz
        double memoryMultiple;
        string schedulingAlgo;
    gates:
        input in;
        output infoOut;        
        output taskFinishedOut;
}


