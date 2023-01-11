1. Start up Docker container
```
docker build -t py-skimmer .
docker run --name=py-skimmer --volume=${PWD}:/workdir --detach py-skimmer
docker exec -it py-skimmer /bin/bash
```
2. Run skim code
```
conda activate cmsql
python3 run.py
```
