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
        int rows;
        int cols ;
        int numUsers;
        double linkDelay;
    submodules:
        edgeServer[rows * cols]: EdgeServer;
        
        finishedTaskCollector: FinishedTaskCollector {
            @display("p=137,150");
        }
        
        users[numUsers]: User {
            @display("p=60,135");
        }

    connections allowunconnected:
        for row=0..rows-1, for col=0..cols-1 {
            edgeServer[row * cols + col].ports++ <--> { delay = 0.005s; } <--> edgeServer[row * cols + col + 1].ports++ if col + 1 < cols;

            edgeServer[row * cols + col].ports++ <--> { delay = 0.005s; } <--> edgeServer[(row + 1) * cols + col].ports++ if row + 1 < rows;
        	
        	edgeServer[row * cols + col].ports++ <--> { delay = 0.005s; } <--> edgeServer[(row + 1) * cols + col + 1].ports++ if row + 1 != rows && col + 1 < cols; 
	        
	        edgeServer[row * cols + col].ports++ <--> { delay = 0.005s; } <--> edgeServer[(row + 1) * cols + col - 1].ports++ if row + 1 != rows && col - 1 >= 0; 
        	
        }
        for i=0..rows * cols - 1 {
            edgeServer[i].taskFinishedOut --> IdealChannel --> finishedTaskCollector.in++;
        }
}
