# She-bang!
#!/bin.bash

export CONVENIENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source global_vars.sh

cd BuildGenerators

source build_generators_gpvm.sh

cd $CONVENIENT_DIR
