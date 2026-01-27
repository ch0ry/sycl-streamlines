#!/bin/bash

CONTAINER_NAME="hip-container"
HOST_OUTPUT_DIR="./results"

docker run --name "$CONTAINER_NAME" --runtime=amd --gpus all ch0ry/hip-test:latest

docker cp "$CONTAINER_NAME":"/test-program/results.csv" "$HOST_OUTPUT_DIR"/

docker rm "$CONTAINER_NAME"
