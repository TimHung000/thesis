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
simple FinishedTaskCollector
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
        @signal[hopCount](type="long");
        @signal[isCompleted](type="bool");
        @statistic[lifeTime](title="lifetime of arrived jobs";source="lifeTime";record=vector);
        @statistic[totalWaitingTime](title="the total time spent in waiting queue";source="totalWaitingTime";record=vector);
        @statistic[totalProcessingTime](title="the total time spent in processing the task";source="totalProcessingTime";record=vector);
        @statistic[totalPropagationTime](title="the total time spent on the link";source="totalPropagationTime";record=vector);
        @statistic[deadline](title="the task maximum delay allowed";source="deadline";record=vector);
        @statistic[taskSize](title="the size of the task";source="taskSize";record=vector);
        @statistic[cpuCycles](title="the required cpu cycles to process the task";source="cpuCycles";record=vector);
        @statistic[processedCycles](title="the processed cycles of the task";source="processedCycles";record=vector);
        @statistic[arrivingServer](title="starting server of task";source="arrivingServer";record=vector);
        @statistic[runningServer](title="running server of task";source="runningServer";record=vector);    
        @statistic[hopCount](title="hop count of task";source="hopCount";record=vector);
        @statistic[isCompleted](title="task status"; source="isCompleted";record=vector);   
        
    gates:
        input in[];
}