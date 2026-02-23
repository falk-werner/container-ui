#!/usr/bin/env bash

docker run -it --rm -p 8888:8888 -v /var/run/docker.sock:/var/run/docker.sock container-ui
