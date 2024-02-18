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

#ifndef SERVERSTATUS_H_
#define SERVERSTATUS_H_

class ServerStatus {
private:
    int serverId;
    double serverFrequency;
    double serverMemory;
    double totalRequiredCycle;
    double totalMemoryConsumed;

public:
    ServerStatus(int serverId, double serverFrequency, double serverMemory,
            double totalRequiredCycle, double totalMemoryConsumed);
    virtual ~ServerStatus();
    void setServerId(int severId);
    int getServerId();
    void setServerFrequency(double serverFrequency);
    double getServerFrequency();
    void setServerMemory(double serverMemory);
    double getServerMemory();
    void setTotalRequiredCycle(double totalRequiredCycle);
    double getTotalRequiredCycle();
    void setTotalMemoryConsumed(double totalMemoryConsumed);
    double getTotalMemoryConsumed();
};

#endif /* SERVERSTATUS_H_ */
