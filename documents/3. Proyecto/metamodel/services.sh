#!/bin/bash

docker kill patio-looker house-looker
docker container rm patio-looker house-looker

docker run -d -p 3000:3000 --name=patio-looker --network=smart_campus_production_smart_uis -e APP_NAME=Patio -e FORWARD_INTERVAL=10  mrdahaniel/looker:latest

docker run -d -p 3001:3000 --name=house-looker --network=smart_campus_production_smart_uis -e APP_NAME=House -e FORWARD_INTERVAL=10  mrdahaniel/looker:latest

docker kill termo monoxide combo-sense
docker container rm termo monoxide combo-sense

docker run -d -e RUST_LOG=mocker -e ip=mqtt_broker -e device_uuid=$(uuidgen) --name=termo --network=smart_campus_production_smart_uis mrdahaniel/mocker:latest temperature:random\(28,30\) location:first-floor interval:20

docker run -d -e RUST_LOG=mocker -e ip=mqtt_broker -e device_uuid=$(uuidgen) --name=monoxide --network=smart_campus_production_smart_uis mrdahaniel/mocker:latest co:random\(0,500\) location:basement interval:20

docker run -d -e RUST_LOG=mocker -e ip=mqtt_broker -e device_uuid=$(uuidgen) --name=combo-sense --network=smart_campus_production_smart_uis mrdahaniel/mocker:latest co2:random\(0,500\) humidity:fixed\(50\) temperature:random\(30,35\) location:greenhouse interval:20

