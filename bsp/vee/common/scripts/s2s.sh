#!/bin/bash

set -e
set -x

S2S_TTY=${1}
S2S_PORT=${2}

if [ "$(ss -a  | grep "\<${S2S_PORT}\>")" == "" ]
then
    cd "$(dirname $(realpath $0))/../../../../"
    /bin/bash gradlew execTool \
        --name=serialToSocketTransmitter \
        --toolProperty=serail.to.socket.comm.port="${S2S_TTY}" \
        --toolProperty=serail.to.socket.comm.baudrate="115200" \
        --toolProperty=serail.to.socket.server.port="${S2S_PORT}"
fi
