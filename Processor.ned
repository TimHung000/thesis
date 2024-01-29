simple Processor
{
    parameters:
        @display("i=block/server");
        @signal[busy](type="bool");
        @signal[cpuFrequency](type="double");
        @statistic[busy](title="server busy state";record=vector);
        @statistic[cpuFrequency](title="cpu frequency";record=vector);
    gates:
        input in;
        output infoOut;        
        output taskCompleteOut;
        output taskDropOut;
}


