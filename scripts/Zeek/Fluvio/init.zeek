##! Zeek integration for Fluvio Log Writer.

module Fluvio;

export {
    ## Define the log writer type for Fluvio natively bound by Zeek Event Engine.
    const LOG: Log::Writer = Log::WRITER_FLUVIOWRITER &redef;

    ## Override default topic name (if empty, dynamically maps to log path like `conn`).
    const default_topic: string = "" &redef;
}
