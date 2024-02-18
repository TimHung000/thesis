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

#include "ServerStatus.h"

ServerStatus::ServerStatus(int serverId, double serverFrequency, double serverMemory,
        double totalRequiredCycle, double totalMemoryConsumed) {
    this->serverId = serverId;
    this->serverFrequency = serverFrequency;
    this->serverMemory = serverMemory;
    this->totalRequiredCycle = totalRequiredCycle;
    this->totalMemoryConsumed = totalMemoryConsumed;
}

ServerStatus::~ServerStatus() {
    // TODO Auto-generated destructor stub
}

void ServerStatus::setServerId(int severId) {
    this->serverId = severId;
}

int ServerStatus::getServerId() {
    return this->serverId;
}

void ServerStatus::setServerFrequency(double serverFrequency) {
    this->serverFrequency = serverFrequency;
}

double ServerStatus::getServerFrequency() {
    return this->serverFrequency;
}

void ServerStatus::setServerMemory(double serverMemory) {
    this->serverMemory = serverMemory;
}

double ServerStatus::getServerMemory() {
    return this->serverMemory;
}

void ServerStatus::setTotalRequiredCycle(double totalRequiredCycle) {
    this->totalRequiredCycle = totalRequiredCycle;
}

double ServerStatus::getTotalRequiredCycle() {
    return this->totalRequiredCycle;
}

void ServerStatus::setTotalMemoryConsumed(double totalMemoryConsumed) {
    this->totalMemoryConsumed = totalMemoryConsumed;
}

double ServerStatus::getTotalMemoryConsumed() {
    return this->totalMemoryConsumed;
}

