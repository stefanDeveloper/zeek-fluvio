##! Stream binding logic to pipe logs seamlessly to Fluvio streams.
@load ./init.zeek

module Fluvio;

event zeek_init() &priority=-10 {
    for (stream_id in Log::active_streams) {
        if (send_all_active_logs || stream_id in logs_to_send) {
            if (stream_id !in logs_to_exclude) {
                # Fallback to explicit topic name if the admin heavily dictates it
                local target_path = (default_topic_name == "") ? fmt("%s", stream_id) : default_topic_name;
                
                local filter: Log::Filter = [
                    $name = fmt("fluvio-%s", stream_id),
                    $writer = Log::WRITER_FLUVIOWRITER,
                    $path = target_path
                ];
                Log::add_filter(stream_id, filter);
            }
        }
    }
}
