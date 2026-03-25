# Zeek-Fluvio Plugin

A high-performance Zeek log writer plugin that bridges telemetry payloads seamlessly into **Fluvio** streams. Built natively on the [Fluvio C++ Client](https://github.com/infinyon/fluvio-client-cpp), this plugin ensures low-latency data ingestion for security operations and network analysis.

## 🚀 Features

- **Native C++ Performance**: Leverages the official Fluvio C++ SDK for high-throughput log shipping.
- **Selective Log Streaming**: Fine-grained control over which logs are sent to Fluvio via Zeek scripts.
- **Automatic Topic Mapping**: Dynamically maps Zeek log streams (e.g., `conn`, `dns`, `http`) to Fluvio topics.
- **TLS Support**: Programmatic mTLS authentication support for secure data pipelines.

## 🛠️ Installation

### Using Zeek Package Manager (zkg)

The easiest way to install the plugin is via `zkg`:

```bash
zkg install zeek-fluvio
```

### Manual Build

If you prefer to build from source:

```bash
git clone https://github.com/infinyon/zeek-fluvio
cd zeek-fluvio
./configure --zeek-dist=/path/to/zeek/source
make -C build
sudo make -C build install
```

> [!NOTE]
> The build system uses `FetchContent` to automatically download the correct version of the Fluvio C++ client for your architecture.

## ⚙️ Configuration

The plugin is configured via the `Fluvio` module in Zeek. Add the following to your `local.zeek` or custom policy scripts:

### Basic Setup

To send all active Zeek logs to Fluvio:

```zeek
redef Fluvio::send_all_active_logs = T;
```

### Selective Logging

To send only specific logs (e.g., Connection and DNS logs):

```zeek
redef Fluvio::logs_to_send = set(Conn::LOG, DNS::LOG);
```

### Excluding Logs

To exclude sensitive logs from being streamed:

```zeek
redef Fluvio::logs_to_exclude = set(Software::LOG);
```

### Options Reference

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `Fluvio::send_all_active_logs` | `bool` | `F` | If `T`, sends every active log stream to Fluvio. |
| `Fluvio::logs_to_send` | `set[Log::ID]` | `{}` | Explicit set of logs to ship. |
| `Fluvio::logs_to_exclude` | `set[Log::ID]` | `{}` | Logs that should never be sent to Fluvio. |
| `Fluvio::default_topic_name` | `string` | `""` | Overrides the default topic mapping (uses log path if empty). |

## 🏗️ Architecture

The plugin implements a custom `Log::Writer` called `FLUVIOWRITER`. When a log record is written in Zeek, the plugin:
1. Formats the record as JSON.
2. Identifies the target Fluvio topic (defaulting to the log's path).
3. Asynchronously dispatches the payload to the Fluvio SPU clusters.
