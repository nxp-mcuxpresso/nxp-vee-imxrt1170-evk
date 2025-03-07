#!/bin/bash

# 'clean.sh' implementation
# 'clean.sh' is responsible for cleaning the project

set -euo pipefail

source $(dirname $(realpath $0))/"set_project_env.sh" || exit 1

cd $MAKEFILE_DIR/../armgcc|| exit 2

. "clean.sh" || exit 3

cd $CURRENT_DIRECTORY || exit 4
