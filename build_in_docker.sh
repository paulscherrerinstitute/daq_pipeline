#!/usr/bin/env bash

VERSION=0.0.1

docker login

docker build -f docker/Dockerfile --no-cache=true -t paulscherrerinstitute/daq_pipeline .
#docker tag paulscherrerinstitute/daq_pipeline_base paulscherrerinstitute/daq_pipeline:$VERSION

#docker push paulscherrerinstitute/daq_pipeline:$VERSION
#docker push paulscherrerinstitute/daq_pipeline
