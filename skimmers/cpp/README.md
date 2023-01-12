    docker build -t cpp-skimmer .
    docker run --name=cpp-skimmer --volume=${PWD}:/workdir --detach cpp-skimmer
    docker exec -it cpp-skimmer /bin/bash
