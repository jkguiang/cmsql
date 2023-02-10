FROM rootproject/root:6.22.08-centos7

# Install CAs
RUN yum install -y https://repo.opensciencegrid.org/osg/3.6/osg-3.6-el7-release-latest.rpm
RUN yum --disablerepo=* --enablerepo=osg install -y osg-ca-certs 

COPY ./main.cc ./Makefile /workdir/
COPY x509up_u31800 /tmp/x509_proxy

RUN useradd -ms /bin/bash jguiang
RUN chown jguiang:jguiang /tmp/x509_proxy

RUN mkdir -p /workdir && chown jguiang:jguiang /workdir

USER jguiang

ENV X509_USER_PROXY='/tmp/x509_proxy'

# Set up working directory

WORKDIR /workdir

ENTRYPOINT ["tail" ,"-f", "/dev/null"]
