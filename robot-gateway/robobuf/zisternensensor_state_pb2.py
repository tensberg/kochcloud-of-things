# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: robobuf/zisternensensor_state.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from robobuf import device_status_pb2 as robobuf_dot_device__status__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n#robobuf/zisternensensor_state.proto\x12\x07robobuf\x1a\x1brobobuf/device_status.proto\"\xab\x01\n\x13ZisternensensorData\x12\x18\n\x0btemperature\x18\x01 \x01(\x01H\x00\x88\x01\x01\x12\x15\n\x08pressure\x18\x02 \x01(\x01H\x01\x88\x01\x01\x12\x15\n\x08humidity\x18\x03 \x01(\x01H\x02\x88\x01\x01\x12\x15\n\x08\x64istance\x18\x04 \x01(\rH\x03\x88\x01\x01\x42\x0e\n\x0c_temperatureB\x0b\n\t_pressureB\x0b\n\t_humidityB\x0b\n\t_distance\"i\n\x14ZisternensensorState\x12*\n\x04\x64\x61ta\x18\x01 \x01(\x0b\x32\x1c.robobuf.ZisternensensorData\x12%\n\x06status\x18\x02 \x01(\x0b\x32\x15.robobuf.DeviceStatusb\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'robobuf.zisternensensor_state_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _ZISTERNENSENSORDATA._serialized_start=78
  _ZISTERNENSENSORDATA._serialized_end=249
  _ZISTERNENSENSORSTATE._serialized_start=251
  _ZISTERNENSENSORSTATE._serialized_end=356
# @@protoc_insertion_point(module_scope)