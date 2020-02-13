#!/usr/bin/env bash

VERSION=0.0.1

cp ../../operational_tools/sources/sources_all.json sources.json

docker login

docker build -f Dockerfile_generator --no-cache=true -t paulscherrerinstitute/daq_pipeline_generator .
docker tag paulscherrerinstitute/daq_pipeline_generator paulscherrerinstitute/daq_pipeline_generator:$VERSION

rm sources.json

docker push paulscherrerinstitute/daq_pipeline_generator:$VERSION
docker push paulscherrerinstitute/daq_pipeline_generator