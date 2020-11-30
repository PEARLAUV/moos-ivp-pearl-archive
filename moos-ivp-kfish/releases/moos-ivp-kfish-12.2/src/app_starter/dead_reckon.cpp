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
 *  File: dead_reckon.cpp
 *  Desc: Interactively controller the orientation and translation of the robot,
 *        using the encoders for feedback.
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
#include <string>
#include <sstream>
#include <stdio.h>  
#include <math.h>
#include <fcntl.h>    // for stdin modesetting, non-blocking reads
#include <signal.h>   // so we can exit cleanly on fatal signals
#include <sys/time.h> // for ms resolution wall time

#include "clearpath.h"
#include "dead_reckon.h"

using namespace std;
using namespace clearpath;

DeadReckon *controller = 0;

void teardown() 
{
    if(controller) delete controller;
}

/* Used to turn fatal signals into normal exits, which ensures that 
 * KeyboardController gets destroyed properly */
void termSignalHandler(int sig)
{
    sig = 0;  // Go away, warning.  Param is required by spec.
    exit(0);
}

int main(int argc, char *argv[])
{
    /* Configure the serial port */
    const char* port = (argc == 2) ? argv[1] : "/dev/ttyUSB0";
    clearpath::Transport::instance().configure(port, 3 /* max retries*/);

    /* Print a nice welcome & some robot info */
    cout << "\n=== Clearpath Robot Dead Reckoner ===" << endl;
    DataPlatformName *robotName = DataPlatformName::getUpdate();
    cout << "Robot: " << robotName->getName() << endl;
    DataPlatformInfo *robotInfo = DataPlatformInfo::getUpdate();
    cout << "Model: " << robotInfo->getModel() << endl;
    cout << "\n'help' to list commands, 'quit' to exit" << endl;
    cout.precision(3);

    /* Ensure we exit cleanly on fatal signals. */
    signal(SIGINT, termSignalHandler);
    signal(SIGTERM, termSignalHandler);

    /* NB: We need to be careful to actually destroy controller at program
     * exit, since its destructor does some important clean-up */
    controller = new DeadReckon();
    atexit(teardown);

    /* Enter main loop */
    controller->spin();
}

DeadReckon::DeadReckon() :
    m_blockingStdin(true), // We assume that stdin starts in blocking mode!
    m_lastEncoderMS(timeMillis())
{
    m_stdinOrigFlags = fcntl(0, F_GETFL);
}

DeadReckon::~DeadReckon()
{
    setStdinBlocking(true);
    cout << "\nExited" << endl;
}

/* Handle the 'forward' command.
 * @param args  A stringstream advanced to the beginning of the arguments
 *              given for the command. */
void DeadReckon::forwardCmd(stringstream & args) 
{
    double distance;
    args >> distance;
    if( args.fail() ) {
        cout << "Distance must be specifid as a valid floating point value!" << endl;
        return;
    }
    cout << "Moving forward " << distance << "m." << endl;
    cout << "Press enter to abort." << endl;

    /* Set up encoder data subscription */
    Transport::instance().flush(DataEncoders::getTypeID());  // ensure no old encoder data is queued.
    DataEncoders::subscribe(ENCODER_SUBS_FREQ);

    /* Obtain initial encoder data */
    DataEncoders * enc = DataEncoders::waitNext(0.2);
    if( ! enc ) {
        cout << "Failed to obtain encoder data!" << endl;
        return;
    }
    if( enc->getCount() != 2 ) {
        cout << "Expected 2 encoders, but found " << enc->getCount() << endl;
        delete enc;
        return;
    }
    double start_dist = (enc->getTravel(0) + enc->getTravel(1)) / 2.0;
    delete enc;

    /* We want to simulteneously monitor stdin and receive encoders messages.
     * So, set stdin non-blocking so we can poll */
    setStdinBlocking(false);
    long lastEnc = timeMillis();
    char curCh;
    while(1) {
        /* Check for keyboard input.  Input of any kind is an abort request 
         * NB: We don't disable linebuffering, so user must press enter */
        if( read(0, &curCh, 1) == 1 ) {
            cout << "Aborted command." << endl;
            return;
        }

        // Check if any encoder data has arrived
        enc = DataEncoders::popNext();

        if( !enc ) {
            // No encoder data
            if( timeMillis() - lastEnc > 200 ) {
                // Waited 200ms, something is wrong.
                cout << "Timed out waiting for encoder data." << endl;
                return;
            } else {
                usleep(1000); // don't loop too tightly
                continue;
            }
        }

        lastEnc = timeMillis();

        /* Calculate distance from start point */
        double curDist = (enc->getTravel(0) + enc->getTravel(1)) / 2.0;
        curDist -= start_dist;
        delete enc;

        cout << "\rError: " << (distance-curDist) << "m.  " << flush;

        /* Check if we've reached our setpoint */
        if( fabs(distance-curDist) < DIST_TOL ) {
            cout << "Done!" << endl;
            SetVelocity(0.0,0.0,0.5).send();
            return;
        }

        /* Set speed proportional to our distance from the setpoint */
        double velocity = distance - curDist;
        if( velocity > MAX_DRIVE_SPEED ) velocity = MAX_DRIVE_SPEED;
        if( velocity < -MAX_DRIVE_SPEED ) velocity = -MAX_DRIVE_SPEED;

        SetVelocity(velocity,0.0,0.2).send();

    }
}

/* Handle the 'turn' command.
 * @param args  A stringstream advanced to the beginning of the arguments 
 *              given for the command. */
void DeadReckon::turnCmd(stringstream & args)
{
    double angle;
    args >> angle;
    if( args.fail() ) {
        cout << "Angle must be specified as a valid floating point value!" << endl;
        return;
    }
    cout << "Turning " << angle << " degrees counterclockwise." << endl;
    cout << "Press enter to abort." << endl;

    /* Set up encoder data subscription */
    Transport::instance().flush(DataEncoders::getTypeID()); // make sure no old encoder data is buffered
    DataEncoders::subscribe(ENCODER_SUBS_FREQ);

    /* Obtain initial encoder data */
    DataEncoders * enc = DataEncoders::waitNext(0.2);
    if( ! enc ) {
        cout << "Failed to obtain encoder data!" << endl;
        return;
    }
    if( enc->getCount() != 2 ) {
        cout << "Expected 2 encoders, but found " << enc->getCount() << endl;
        delete enc;
        return;
    }
    double leftStart = enc->getTravel(0);
    double rightStart = enc->getTravel(1);
    delete enc;

    /* Set stdin non-blocking so we can poll it and the robot */
    setStdinBlocking(false);
    long lastEnc = timeMillis();
    char curCh;
    while(1) {
        /* Check for keyboard input.  Input of any sort is an abort request.
         * NB: We don't disable linebuffering, so user must press 'enter' */
        if( read(0, &curCh, 1) == 1 ) {
            cout << "Aborted command." << endl;
            return;
        }

        // Check if any encoder data has arrived
        enc = DataEncoders::popNext();

        if( !enc ) {
            // No encoder data
            if( timeMillis() - lastEnc > 200 ) {
                // Waited 200ms, something is wrong.
                cout << "Timed out waiting for encoder data." << endl;
                return;
            } else {
                usleep(1000); // don't loop too tightly
                continue;
            }
        }

        lastEnc = timeMillis();

        /* Calculate the angle we've turned through */
        double curLeft = enc->getTravel(0) - leftStart;
        double curRight = enc->getTravel(1) - rightStart;
        double curAngle = (curRight - curLeft) / PLATFORM_WIDTH;
        curAngle = curAngle * 180.0 / M_PI; // convert to deg

        cout << "\rError: " << (angle-curAngle) << "deg.  " << flush;

        /* Check if we've reached the setpoint */
        if( fabs(curAngle-angle) < ANGLE_TOL ) {
            cout << "Done!" << endl;
            SetVelocity(0,0,0.5).send();
            return;
        }

        /* Set angular apeed proportional to error */
        // Back to rad, with an arbitrary gain:
        double velocity = (angle-curAngle) * (M_PI / 180.0) * 1.5;  
        if( velocity > MAX_TURN_SPEED ) velocity = MAX_TURN_SPEED;
        if( velocity < -MAX_TURN_SPEED ) velocity = -MAX_TURN_SPEED;
        
        SetVelocity(0.0,velocity,0.2).send();
    }
}



void DeadReckon::setStdinBlocking(bool block)
{
    if( block == m_blockingStdin ) return;
    if( block ) {
        // Restore the blocking flags
        if( fcntl(0, F_SETFL, m_stdinOrigFlags) ) {
            perror("fcntl");
            exit(1);
        }
        m_blockingStdin = true;
    } else {
        // Set non-blocking mode
        if( fcntl(0, F_SETFL, m_stdinOrigFlags | O_NONBLOCK ) ) {
            perror("fcntl");
            exit(1);
        }
        m_blockingStdin = false;
    }
}

/* Main application loop.  Reads commands from stdin and controls
 * the robot. */
void DeadReckon::spin()
{
    string lineIn;
    while(1) {
        /* We are about to wait for input. Make sure the robot is stopped
         * and the input buffer is not slowly filling with encoder messages */
        SetVelocity(0,0,1.0).send();
        DataEncoders::subscribe(0xffff);

        /* Wait for a line of input */
        setStdinBlocking(true);
        cout << ">>> " << flush;
        getline(cin, lineIn);
        
        /* Have input from user, parse it! */
        stringstream lineStream (lineIn);
        string cmd;
        lineStream >> cmd;
        if( cmd == "" ) {
            continue;
        } else if( cmd == "quit" || cmd == "exit" ) {
            exit(0);
        } else if( cmd == "help" || cmd == "h" ) {
            help();
        } else if( cmd == "forward" ) {
            forwardCmd(lineStream);
        } else if( cmd == "turn" ) {
            turnCmd(lineStream);
        } else {
            cout << "Unrecognized command" << endl;
        }
    }
}

void DeadReckon::help()
{
    cout << "=== Available Commands ===" << endl;
    cout << "quit,exit  : Exit this program, stopping the robot." << endl;
    cout << "help,h     : Print this help" << endl;
    cout << "forward [d]: Move the robot [d] meters forward." << endl;
    cout << "turn [o]   : Rotate the robot [o] degrees counter-clockwise." << endl;
}

/* Get wall millis since program start */
long DeadReckon::timeMillis()
{
    static bool firstRun = true;
    static struct timeval startTime;
    struct timeval tv;
    struct timezone tz;

    if(firstRun) {
        gettimeofday(&startTime, &tz); 
        firstRun = false;
    }

    gettimeofday(&tv, &tz);

    return (tv.tv_sec - startTime.tv_sec)*1000 
           + (tv.tv_usec - startTime.tv_usec)/1000;
}

