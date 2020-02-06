#!/usr/bin/env bash

VERSION=0.0.1

docker login

docker build -f Dockerfile_base --no-cache=true -t paulscherrerinstitute/daq_pipeline_base .
docker tag paulscherrerinstitute/daq_pipeline_base paulscherrerinstitute/daq_pipeline_base:$VERSION

docker push paulscherrerinstitute/daq_pipeline_base:$VERSION
docker push paulscherrerinstitute/daq_pipeline_base
