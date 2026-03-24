#!/bin/bash
set -e

PCAP_FILE=${1}
if [ -z "$PCAP_FILE" ] || [ ! -f "$PCAP_FILE" ]; then
    echo "Usage: ./tests/e2e.sh <path_to_pcap.pcap>"
    exit 1
fi

echo "[E2E] Bootstrapping Fluvio Topic..."
export PATH="$HOME/.fluvio/bin:/opt/zeek/bin:$PATH"
export ZEEK_PLUGIN_PATH=$(pwd)/build

fluvio topic delete zeek-trace || true
sleep 1
fluvio topic create zeek-trace

echo "[E2E] Creating Zeek Intercept Script..."
cat << 'EOF' > tests/test_e2e.zeek
@load base/protocols/conn
@load Zeek/Fluvio
redef Fluvio::default_topic_name = "zeek-trace";
redef Fluvio::send_all_active_logs = T;
EOF

echo "[E2E] Running Zeek against provided PCAP: $PCAP_FILE"
zeek -r "$PCAP_FILE" tests/test_e2e.zeek

sleep 3

echo "[E2E] Verifying Fluvio Stream Consumption..."
fluvio consume zeek-trace -B -d > tests/consumed.json

if grep -q "id.orig_h" tests/consumed.json; then
    echo "[E2E] SUCCESS! Network trace was successfully serialized and routed through Fluvio!"
    cat tests/consumed.json
    exit 0
else
    echo "[E2E] FAILURE. Fluvio topic did not receive the JSON payload."
    exit 1
fi
