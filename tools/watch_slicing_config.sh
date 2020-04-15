#!/bin/bash

host=localhost
port=9999
interval=1

watch -n$interval "curl -sX GET http://$host:$port/stats | jq -f slicing.jq"
