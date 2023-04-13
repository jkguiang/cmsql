    docker build -t cpp-skimmer .
    docker run --name=cpp-skimmer --detach cpp-skimmer
    docker exec -it cpp-skimmer /bin/bash

    # May need to change the path to the input root file

    make -j
    ./run
