#!/bin/sh

v1name=archie
v2name=betty

shoreip=10.25.0.88
v1ip=10.25.0.92
v2ip=10.25.0.93

shore_port=9000
v1port=9000
v2port=9000

shore_port_udp=9200
v1port_udp=9200
v2port_udp=9200

nsplug meta_vehicle.moos targ_${v1name}.moos -f GPS=iGPS VIP=$v1ip  \
     VNAME=$v1name SIP=$shoreip SPTU=$shore_port_udp SPTT=$shore_port

nsplug meta_vehicle.moos targ_${v2name}.moos -f GPS=iGPS VIP=$v2ip  \
     VNAME=$v2name SIP=$shoreip SPTU=$shore_port_udp SPTT=$shore_port

nsplug meta_shoreside.moos targ_shoreside.moos -f SPTU=$shore_port_udp \
    V1_NAME=$v1name V2_NAME=$v2name  \
    V1_PORT=$v1port V2_PORT=$v2port  \
    V1_IP=$v1ip V2_IP=$v2ip  \

