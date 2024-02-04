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
import ned.IdealChannel;


network EdgeEnv
{
    parameters:
        @display("bgb=678,418");
        int rows = 5;
        int cols = 5;
        int total = 25;
        edgeServer[0].numUsers = 500;
        edgeServer[24].numUsers = 500;
    submodules:
        edgeServer[rows * cols]: EdgeServer {
            id = index + 1;
        }
        finishedTaskCollector: FinishedTaskCollector {
            @display("p=137,150");
        }
    connections:
        for row=0..rows-1, for col=0..cols-1 {
            edgeServer[row * cols + col].ports++ <--> {  delay = 10ms; } <--> edgeServer[row * cols + col + 1].ports++ if col + 1 != cols;
            edgeServer[row * cols + col].infoPorts++ <--> IdealChannel <--> edgeServer[row * cols + col + 1].infoPorts++ if col + 1 != cols;

            edgeServer[row * cols + col].ports++ <--> {  delay = 10ms; } <--> edgeServer[(row + 1) * cols + col].ports++ if row + 1 != rows;
            edgeServer[row * cols + col].infoPorts++ <--> IdealChannel <--> edgeServer[(row + 1) * cols + col].infoPorts++ if row + 1 != rows;
        }
        for i=0..rows * cols - 1 {
            edgeServer[i].taskFinishedOut --> IdealChannel --> finishedTaskCollector.in++;
        }
}
