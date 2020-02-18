import argparse
import json
import os
import shutil
from copy import deepcopy

START_STREAM_PORT = 10000
DEFAULT_SCYLLA_HOSTS = "172.26.120.72,172.26.120.73,172.26.120.74"

parser = argparse.ArgumentParser()

parser.add_argument('--source_file', type=str, default="sources/sources_all.json",
                    help='File with the desired sources.')
parser.add_argument('--scylla_hosts', type=str, default=DEFAULT_SCYLLA_HOSTS,
                    help='Scylla hosts to access the data.')
parser.add_argument('--generator_host', type=str, default="172.26.120.71",
                    help='Host where the generator code will be running.')

args = parser.parse_args()

if os.path.exists("daq_pipelines/"):
    shutil.rmtree("daq_pipelines/")

if os.path.exists("generators/"):
    shutil.rmtree("generators/")

os.mkdir("daq_pipelines/")
os.mkdir("generators/")

with open("template_daq_pipeline.json") as input_file:
    template_daq_pipeline = json.load(input_file)

with open("template_generator.json") as input_file:
    template_generator = json.load(input_file)

with open(args.source_file) as input_file:
    data = json.load(input_file)

docker_command = 'docker run --net=host -e DEVICE_NAME="%s" -e PORT=%s paulscherrerinstitute/daq_pipeline_generator &'
temp = []

for index, device in enumerate(data.keys()):
    daq_pipeline_file = deepcopy(template_daq_pipeline)
    generators_file = deepcopy(template_generator)

    stream_port = START_STREAM_PORT + index
    stream_address = "tcp://%s:%s" % (args.generator_host, stream_port)

    daq_pipeline_file["metadata"]["name"] = device.lower()
    generators_file["metadata"]["name"] = device.lower()

    daq_pipeline_file["spec"]["containers"][0]["env"]\
        .append({"name": "SCYLLA_HOSTS", "value": args.scylla_hosts})
    daq_pipeline_file["spec"]["containers"][0]["env"] \
        .append({"name": "STREAM_HOST", "value": stream_address})

    generators_file["spec"]["containers"][0]["env"] \
        .append({"name": "DEVICE_NAME", "value": device})
    generators_file["spec"]["containers"][0]["env"] \
        .append({"name": "PORT", "value": stream_port})

    temp.append((device,docker_command % (device, stream_port)))

    with open("daq_pipelines/%s.json" % device, 'w') as output_file:
        json.dump(daq_pipeline_file, output_file, indent=4)

    with open("generators/%s.json" % device, 'w') as output_file:
        json.dump(generators_file, output_file, indent=4)

temp = sorted(temp, key=lambda x:x[0])
with open("generators/start_generators.sh", 'w') as output_file:
    output_file.write("\n".join((x[1] for x in temp)))