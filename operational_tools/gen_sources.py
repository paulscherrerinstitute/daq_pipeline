import argparse
import json
import os
import shutil
from copy import deepcopy
from collections import defaultdict

START_STREAM_PORT = 10000
DEFAULT_SCYLLA_HOSTS = "172.26.120.72,172.26.120.73,172.26.120.74"

GENERATOR_HOSTS = ["sf-daqstress-01", "sf-daqstress-02"]

parser = argparse.ArgumentParser()

parser.add_argument('--source_file', type=str, default="sources/sources_all.json",
                    help='File with the desired sources.')
parser.add_argument('--scylla_hosts', type=str, default=DEFAULT_SCYLLA_HOSTS,
                    help='Scylla hosts to access the data.')

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

docker_command = ' run --net=host -e DEVICE_NAME="%s" -e PORT=%s paulscherrerinstitute/daq_pipeline_generator &'
bash_generators = defaultdict(list)

for index, device in enumerate(data.keys()):
    daq_pipeline_file = deepcopy(template_daq_pipeline)
    generators_file = deepcopy(template_generator)

    generator_host = GENERATOR_HOSTS[index % len(GENERATOR_HOSTS)]

    stream_port = START_STREAM_PORT + index
    stream_address = "tcp://%s:%s" % (generator_host, stream_port)

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

    bash_generators[generator_host].append((device,docker_command % (device, stream_port)))

    with open("daq_pipelines/%s.json" % device, 'w') as output_file:
        json.dump(daq_pipeline_file, output_file, indent=4)

    with open("generators/%s.json" % device, 'w') as output_file:
        json.dump(generators_file, output_file, indent=4)

for key, value in bash_generators.items():

    temp = sorted(value, key=lambda x:x[0])

    with open("generators/%s.sh" % key, 'w') as output_file:
        output_file.write("\n".join((x[1] for x in temp)))