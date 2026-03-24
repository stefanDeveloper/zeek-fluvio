##! Zeek integration for Fluvio Log Writer.

module Fluvio;

export {
    ## Define the log writer type for Fluvio natively bound by Zeek Event Engine.
    const LOG: Log::Writer = Log::WRITER_FLUVIOWRITER &redef;

    ## Send all active logs to Fluvio except for those explicitly excluded.
    ## Example: redef Fluvio::send_all_active_logs = T;
    const send_all_active_logs: bool = F &redef;

    ## Specify which Log::ID to send to Fluvio dynamically.
    ## Example: redef Fluvio::logs_to_send = set(Conn::LOG, DNS::LOG);
    const logs_to_send: set[Log::ID] &redef;

    ## Specify which Log::ID to unequivocally securely exclude from Fluvio streams.
    const logs_to_exclude: set[Log::ID] &redef;

    ## Default destination Fluvio Topic. If empty, uses the stream path (e.g. 'conn', 'http').
    const default_topic_name: string = "" &redef;
}
