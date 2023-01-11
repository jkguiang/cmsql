docker build -t py-skimmer .
docker run --name=py-skimmer --volume=${PWD}:/workdir --detach py-skimmer
docker exec -it py-skimmer /bin/bash
