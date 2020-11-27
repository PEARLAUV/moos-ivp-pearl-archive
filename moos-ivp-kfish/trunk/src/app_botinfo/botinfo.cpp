/**
 *      _____
 *     /  _  \
 *    / _/ \  \
 *   / / \_/   \
 *  /  \_/  _   \  ___  _    ___   ___   ____   ____   ___   _____  _   _
 *  \  / \_/ \  / /  _\| |  | __| / _ \ | ++ \ | ++ \ / _ \ |_   _|| | | |
 *   \ \_/ \_/ /  | |  | |  | ++ | |_| || ++ / | ++_/| |_| |  | |  | +-+ |
 *    \  \_/  /   | |_ | |_ | ++ |  _  || |\ \ | |   |  _  |  | |  | +-+ |
 *     \_____/    \___/|___||___||_| |_||_| \_\|_|   |_| |_|  |_|  |_| |_|
 *             ROBOTICS™ 
 *
 *  File: botinfo.cpp
 *  Desc: A simple program which requests various info from the robot, prints,
 *        and exits.
 *  Auth: Iain Peet
 *
 *  Copyright (c) 2010, Clearpath Robotics, Inc. 
 *  All Rights Reserved
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Clearpath Robotics, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CLEARPATH ROBOTICS, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Please send comments, questions, or patches to skynet@clearpathrobotics.com 
 *
 */

#include <iostream>
#include <cstdio>  // want getchar()
#include <cstdlib>
#include <unistd.h>
#include <typeinfo>
#include "clearpath.h"
#include "Message_data.h"
#include "TestUtils.h"

using namespace std;

int main(int argc, char *argv[]) {
  double result = foobar(3, 40);

    /* Configure the serial port */
    const char* port = (argc == 2) ? argv[1] : "/dev/ttyUSB0";
    clearpath::Transport::instance().configure(port, 3 /* max retries*/);

    /* Get and print some system information */
    clearpath::DataPlatformInfo *platform_info = clearpath::DataPlatformInfo::getUpdate();
    cout << *platform_info << endl;
    delete platform_info;

	clearpath::DataFirmwareInfo *fw_info = clearpath::DataFirmwareInfo::getUpdate();
	cout << *fw_info << endl;
	delete fw_info;

    clearpath::DataPowerSystem  *power_info = clearpath::DataPowerSystem::getUpdate();
    cout << *power_info << endl;
    delete power_info;

    clearpath::DataProcessorStatus *cpu_info = clearpath::DataProcessorStatus::getUpdate();
    cout << *cpu_info << endl;
    delete cpu_info;

    return 0;
}

