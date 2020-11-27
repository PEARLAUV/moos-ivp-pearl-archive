#!/bin/bash

# This script permanently sets the Mac's Internet Sharing preferences
# so that the Mac will receive the IP address 192.168.2.1 and the
# attached device will always be assigned 192.168.2.2.
# REQUIRES OS X Yosemite (10.10) or newer
# Internet Sharing must be turned on and off once before this script
# is executed.

sudo defaults write /Library/Preferences/SystemConfiguration/com.apple.nat NAT -dict-add SharingNetworkNumberStart 192.168.2.1
sudo defaults write /Library/Preferences/SystemConfiguration/com.apple.nat NAT -dict-add SharingNetworkNumberEnd 192.168.2.2
sudo defaults write /Library/Preferences/SystemConfiguration/com.apple.nat NAT -dict-add SharingNetworkMask 255.255.255.0
echo "Internet sharing now configured to assign 192.168.2.2"

