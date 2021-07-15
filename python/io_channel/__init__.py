
import io_channel.io_channel as io_channel

def create_channel(node_id, node_cfg, callback):
    channel = io_channel.create_channel(node_id, node_cfg, callback)
    return channel
    