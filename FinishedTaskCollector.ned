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
//        @signal[taskId](type="double");
//        @signal[creationTime](type="simtime_t");
//        @signal[totalWaitingTime](type="simtime_t");
//        @signal[totalProcessingTime](type="simtime_t");
//        @signal[totalPropagationTime](type="simtime_t");
//        @signal[finishedTime](type="simtime_t");
//        @signal[deadline](type="simtime_t");
//        @signal[taskSize](type="double");
//        @signal[requiredCycle](type="double");
//        @signal[processedCycles](type="double");
//        @signal[arrivingServer](type="int");
//        @signal[runningServer](type="int");
//        @signal[hopCount](type="long");
        @signal[isCompleted](type="bool");
//        @signal[totalSubTaskCount](type="int");
		@signal[splitTaskPartialComplete](type="bool");
		@signal[splitTaskComplete](type="bool");
		@signal[totalTime](type="simtime_t");
		@signal[totalReward](type="double");
//        @statistic[taskId](title="task id";source="taskId";record=vector);
//        @statistic[creationTime](title="creation time of arrived jobs";source="creationTime";record=vector);
//        @statistic[totalWaitingTime](title="the total time spent in waiting queue";source="totalWaitingTime";record=vector);
//        @statistic[totalProcessingTime](title="the total time spent in processing the task";source="totalProcessingTime";record=vector);
//        @statistic[totalPropagationTime](title="the total time spent on the link";source="totalPropagationTime";record=vector);
//        @statistic[finishedTime](title="finished time";source="finishedTime";record=vector);
//        @statistic[deadline](title="the task maximum delay allowed";source="deadline";record=vector);
//        @statistic[taskSize](title="the size of the task";source="taskSize";record=vector);
//        @statistic[requiredCycle](title="the required cpu cycles to process the task";source="requiredCycle";record=vector);
//        @statistic[processedCycles](title="the processed cycles of the task";source="processedCycles";record=vector);
//        @statistic[arrivingServer](title="starting server of task";source="arrivingServer";record=vector);
//        @statistic[runningServer](title="running server of task";source="runningServer";record=vector);    
//        @statistic[hopCount](title="hop count of task";source="hopCount";record=vector);
        @statistic[isCompleted](title="task status";source="isCompleted";record=count,sum,mean);
//        @statistic[totalSubTaskCount](title="sub task count";source="totalSubTaskCount";record=vector);
		@statistic[splitTaskPartialComplete](title="splitTaskPartialComplete";source="splitTaskPartialComplete";record=count,sum);
		@statistic[splitTaskComplete](title="splitTaskComplete";source="splitTaskComplete";record=count,sum);
		@statistic[totalTime](title="total time";source="totalTime";record=mean);
		@statistic[totalReward](title="total reward";source="totalReward";record=sum,vector);
    gates:
        input in[];
}