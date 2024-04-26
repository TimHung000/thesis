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

        @signal[isCompleted](type="bool");
		@signal[splitTaskPartialComplete](type="bool");
		@signal[splitTaskComplete](type="bool");
		@signal[taskTime](type="double");
		@signal[reward](type="double");
		@signal[completedDelayTolerance](type="double");
		@signal[delayToToleranceMultiple](type="double");

        @statistic[isCompleted](title="task status";source="isCompleted";record=count,sum,mean);
		@statistic[splitTaskPartialComplete](title="splitTaskPartialComplete";source="splitTaskPartialComplete";record=count,sum);
		@statistic[splitTaskComplete](title="splitTaskComplete";source="splitTaskComplete";record=count,sum);
		@statistic[taskTime](title="task time";source="taskTime";record=sum,mean);
		@statistic[reward](title="reward";source="reward";record=sum);
		@statistic[completedDelayTolerance](title="completed Delay Tolerance";source="completedDelayTolerance";record=sum);
		@statistic[delayToToleranceMultiple](title="delay To Tolerance Multiple";source="delayToToleranceMultiple";record=sum);
    gates:
        input in[];
}