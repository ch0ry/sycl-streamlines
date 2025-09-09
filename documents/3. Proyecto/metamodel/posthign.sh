#!/bin/bash

# Function to make a POST request
function make_post_request {
    endpoint=$1
    json_payload=$2

    curl -X POST -H "Content-Type: application/json" -d "$json_payload" "$endpoint"
}

# Endpoint 1
json_payload_add='{
  "image": "mrdahaniel/mocker:latest",
  "network_name": "smart_campus_production_smart_uis",
  "env_vars": {
    "RUST_LOG": "mocker",
    "ip": "mqtt_broker"
  },
  "args": [
    "key:random(5,35)"
  ]
}'

endpoint1="http://localhost:8014/directives/addition/House/basement"
endpoint2="http://localhost:8014/directives/addition/House/first-floor"
endpoint3="http://localhost:8014/directives/addition/Patio/greenhouse"

make_post_request "$endpoint1" "$json_payload_add"
make_post_request "$endpoint2" "$json_payload_add"
make_post_request "$endpoint3" "$json_payload_add"

json_payload_restart='{
    "uuid": null,
    "query_type": {
        "Http": {
            "endpoint": "/get/device",
            "port": 8000
        }
    }
}'

endpoint1="http://localhost:8014/directives/restart/House/basement"
endpoint2="http://localhost:8014/directives/restart/House/first-floor"
endpoint3="http://localhost:8014/directives/restart/Patio/greenhouse"

make_post_request "$endpoint1" "$json_payload_restart"
make_post_request "$endpoint2" "$json_payload_restart"
make_post_request "$endpoint3" "$json_payload_restart"

json_payload_reconfig='{
    "uuid": null,
    "query_type": {
        "Http": {
            "endpoint": "/get/device",
            "port": 8000
        }
    },
    "network": "smart_campus_production_smart_uis",
    "reconfig": {
        "Http": {
            "endpoint": "/update/interval",
            "port": 8000,
            "method": "PUT",
            "payload": {
                "secs": 25,
                "nanos": 0
            }
        }
    }
}'

endpoint1="http://localhost:8014/directives/reconfig/House/basement"
endpoint2="http://localhost:8014/directives/reconfig/House/first-floor"
endpoint3="http://localhost:8014/directives/reconfig/Patio/greenhouse"

make_post_request "$endpoint1" "$json_payload_reconfig"
make_post_request "$endpoint2" "$json_payload_reconfig"
make_post_request "$endpoint3" "$json_payload_reconfig"