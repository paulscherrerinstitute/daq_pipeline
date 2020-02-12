FROM paulscherrerinstitute/daq_pipeline_base

WORKDIR /daq_pipeline
COPY . /daq_pipeline

SHELL ["/usr/bin/scl", "enable", "devtoolset-7"]

WORKDIR /daq_pipeline/build

RUN cmake3 ..; make;

ENTRYPOINT ["./daq_pipeline"]