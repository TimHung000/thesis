//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple TaskQueue
{
    parameters:
        double memoryThreshold = 0.8;
        int maximumHop = 10;
        string dispatchingAlgo;
        @display("i=abstract/dispatcher");
        @signal[totalTaskDrop](type="double");
        @statistic[totalTaskDrop](title="totalTaskDrop";source="totalTaskDrop";record=vector);
        @signal[memoryLoading](type="double");
        @statistic[memoryLoading](title="double";source="memoryLoading";record=vector);
        @signal[CPULoading](type="double");        
        @statistic[CPULoading](title="double";source="CPULoading";record=vector);
        @signal[totalTaskFinished](type="double");
        @statistic[totalTaskFinished](title="totalTaskFinished";source="totalTaskFinished";record=vector);
        @signal[totalTaskCompleted](type="double");
        @statistic[totalTaskCompleted](title="totalTaskCompleted";source="totalTaskCompleted";record=vector);
        
                
        double statusUpadteInterval;  // ms
        double maxFrequency;          // GHz
        double minFrequency;          // GHz
        double memoryMultiple;
        string schedulingAlgo;
    gates:
        input taskIn[];
        output offloadOut;
        output taskFinishedOut;
}