FROM python:latest

# Install conda
RUN curl -O -L https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
RUN bash Miniconda3-latest-Linux-x86_64.sh -b && rm Miniconda3-latest-Linux-x86_64.sh
# Set temporary env variable to make our lives easier
ENV CONDA=/root/miniconda3/bin/conda
# conda will be on $PATH when you log in
RUN $CONDA init
# Configure conda
RUN $CONDA config --set auto_activate_base false && $CONDA config --add channels conda-forge
# Set up conda environment for RTT tests
RUN $CONDA create --name cmsql -y && \
    $CONDA install -n cmsql -y uproot numpy coffea

RUN mkdir -p /workdir
WORKDIR /workdir

ENTRYPOINT ["tail", "-f", "/dev/null"]
