#/bin/bash

WORKSPACE=
IDE=
PROJECT=rt1170_nxpvee

help() {
    echo "${0}: compile mcuxpressoide projecto from commandline"
    echo "     -w WORKSPACE"
    echo "     -i IDE"
    echo "     -p PROJECT (default ${PROJECT})"
    exit 0
}

while getopts w:i:p:h FLAG
do
    case "${FLAG}" in
        w) WORKSPACE=${OPTARG};;
        i) IDE=${OPTARG};;
        p) PROJECT=${OPTARG};;
        h) help;;
    esac
done

set -ue

"${IDE}" -nosplash --launcher.suppressErrors -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data "${WORKSPACE}" -build "${PROJECT}"/Release
